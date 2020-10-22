#include <variant>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "map_builder.h"
#include "json.h"

using namespace std;

MapBuilder::RenderSettings MapBuilder::MakeRenderSettings(const Json::Dict& json) {
	using Json::Node;
	Svg::Color underlayer_color = DeduceColor(json.at("underlayer_color"));
	const vector<Node>& colorNodes = json.at("color_palette").AsArray();
	vector<Svg::Color> colors;
	colors.reserve(colorNodes.size());
	for (const auto& colorNode : colorNodes) {
		colors.push_back(DeduceColor(colorNode));
	}
	const vector<Node>& SLO_Point = json.at("stop_label_offset").AsArray();
	const vector<Node>& BLO_Point = json.at("bus_label_offset").AsArray();
	return RenderSettings{
		.width = json.at("width").AsDouble(),
		.height = json.at("height").AsDouble(),
		.padding = json.at("padding").AsDouble(),
		.stop_radius = json.at("stop_radius").AsDouble(),
		.line_width = json.at("line_width").AsDouble(),
		.stop_label_font_size = json.at("stop_label_font_size").AsInt(),
		.stop_label_offset = {SLO_Point[0].AsDouble(), SLO_Point[1].AsDouble()},
		.underlayer_color = move(underlayer_color),
		.underlayer_width = json.at("underlayer_width").AsDouble(),
		.color_palette = move(colors),
		.bus_label_font_size = json.at("bus_label_font_size").AsInt(),
		.bus_label_offset = {BLO_Point[0].AsDouble(), BLO_Point[1].AsDouble()},
		.layers_ = MakeLayers(json.at("layers").AsArray())
	};
}

Svg::Color MapBuilder::DeduceColor(const Json::Node& node) {
	using Json::Node;
	if (holds_alternative<string>(node.GetBase())) { // string
		return { node.AsString() };
	}
	else { // rgb or rgba
		const vector<Node>& properties = node.AsArray();
		Svg::Rgb rgb;
		rgb.red = properties[0].AsInt();
		rgb.green = properties[1].AsInt();
		rgb.blue = properties[2].AsInt();
		if (properties.size() == 4u) { // rgba
			rgb.alpha = properties[3].AsDouble();
		}
		return { move(rgb) }; // мув под вопросом
	}
}

void MapBuilder::PrintBusNameAtStop(const string& busName, const string& stopName, size_t colorCounter, Svg::Document& doc) const {
	auto stopPos = stops.at(stopName)->position;
	double longitudeRad = Sphere::ConvertDegreesToRadians(stopPos.longitude);
	double latitudeRad = Sphere::ConvertDegreesToRadians(stopPos.latitude);
	double x = (longitudeRad - misc_.min_lon) * zoomCoef + renderSettings.padding;
	double y = (misc_.max_lat - latitudeRad) * zoomCoef + renderSettings.padding;
	doc.Add( // Подложка + добавить bold!!
		Svg::Text{}
		.SetPoint({x,y})
		.SetOffset(renderSettings.bus_label_offset)
		.SetFontSize(renderSettings.bus_label_font_size)
		.SetFontFamily("Verdana")
		.SetData(busName)
		.SetFontWeight("bold")
		.SetFillColor(renderSettings.underlayer_color)
		.SetStrokeColor(renderSettings.underlayer_color)
		.SetStrokeWidth(renderSettings.underlayer_width)
		.SetStrokeLineCap("round")
		.SetStrokeLineJoin("round")
	);

	size_t idx = colorCounter % renderSettings.color_palette.size();
	const Svg::Color& routeColor = renderSettings.color_palette[idx];

	doc.Add( // Само название + добавить bold!!
		Svg::Text{}
		.SetPoint({ x,y })
		.SetOffset(renderSettings.bus_label_offset)
		.SetFontSize(renderSettings.bus_label_font_size)
		.SetFontWeight("bold")
		.SetFontFamily("Verdana")
		.SetData(busName)
		.SetFillColor(routeColor)
		);
}

void MapBuilder::PrintBusNames(Svg::Document& doc) const {
	size_t counter = 0;
	for (const auto& bus : buses) {
		if (bus.second->isCircular) { // circular
			PrintBusNameAtStop(bus.first, bus.second->stops[0], counter, doc);
		}
		else { // linear
			PrintBusNameAtStop(bus.first, bus.second->stops[0], counter, doc);
			if (bus.second->stops[0] != bus.second->stops[(bus.second->stops.size() - 1) / 2]) {
				PrintBusNameAtStop(
					bus.first,
					bus.second->stops[(bus.second->stops.size() - 1) / 2],
					counter,
					doc
				);
			}
		}
		counter++;
	}

}

void MapBuilder::PrintLayer(Layers layer, Svg::Document& doc) const {
	switch (layer) {
	case Layers::BUS_LABELS:
		PrintBusNames(doc);
		break;
	case Layers::BUS_LINES:
		PrintRoutes(doc);
		break;
	case Layers::STOP_LABELS:
		PrintStopNames(doc);
		break;
	case Layers::STOP_POINTS:
		PrintStopCircles(doc);
		break;
	}
}

void MapBuilder::MakeMap() const {
	Svg::Document img;

	//PrintRoutes(img);
	//PrintBusNames(img);
	//PrintStopCircles(img);
	//PrintStopNames(img);

	for (const auto i : renderSettings.layers_) {
		PrintLayer(i, img);
	}

	ostringstream ss;
	img.Render(ss);

	map_ = ss.str();
}

vector<MapBuilder::Layers> MapBuilder::MakeLayers(const vector<Json::Node>& vec) {
	vector<Layers> toReturn;
	for (const auto& i : vec) {
		const string& asStr = i.AsString();
		if (asStr == "bus_lines") {
			toReturn.push_back(Layers::BUS_LINES);
		}
		else if (asStr == "bus_labels") {
			toReturn.push_back(Layers::BUS_LABELS);
		}
		else if (asStr == "stop_points") {
			toReturn.push_back(Layers::STOP_POINTS);
		}
		else {
			toReturn.push_back(Layers::STOP_LABELS);
		}
	}
	return toReturn; // NRVO вроде?
}

MapBuilder::MapBuilder(
	const Descriptions::StopsDict& stopsDict,
	const Descriptions::BusesDict& busesDict,
	const Json::Dict& renderSetingsJSON,
	const Descriptions::Misc& misc
	) : renderSettings(MakeRenderSettings(renderSetingsJSON)), misc_(misc)
{
	buses = { busesDict.begin(), busesDict.end() };
	stops = { stopsDict.begin(), stopsDict.end() };
	zoomCoef = calculateZoomCoef();
	MakeMap();
}

double MapBuilder::calculateTempZoomCoef(double w_or_h, double min, double max) const {
	return (w_or_h - 2 * renderSettings.padding) / (max - min);
}

double MapBuilder::calculateZoomCoef() const {
	bool first, second;
	first = misc_.max_lon == misc_.min_lon;
	second = misc_.max_lat == misc_.min_lat;
	if (first && !second) {
		return calculateTempZoomCoef(renderSettings.height, misc_.min_lat, misc_.max_lat);
	}
	else if (!first && second) {
		return calculateTempZoomCoef(renderSettings.width, misc_.min_lon, misc_.max_lon);
	}
	else if (first && second) {
		return 0.;
	}
	else {
		return std::min(
			calculateTempZoomCoef(renderSettings.height, misc_.min_lat, misc_.max_lat),
			calculateTempZoomCoef(renderSettings.width, misc_.min_lon, misc_.max_lon)
		);
	}
}

void MapBuilder::PrintRoutes(Svg::Document& doc) const {
	size_t counter = 0;
	for (const auto& bus : buses) {
		size_t idx = counter % renderSettings.color_palette.size();
		const Svg::Color& routeColor = renderSettings.color_palette[idx]; //!!

		const auto& stopsTemp = bus.second->stops;

		Svg::Polyline toAdd{};
		toAdd.
			SetStrokeColor(routeColor).
			SetStrokeWidth(renderSettings.line_width).
			SetStrokeLineCap("round").
			SetStrokeLineJoin("round");

		for (size_t i = 0; i < stopsTemp.size(); i++) {
			const Sphere::Point& pointTemp = stops.at(stopsTemp[i])->position;
			//const Sphere::Point& toPoint = stops.at(stopsTemp[i + 1])->position;
			double longitudeRad = Sphere::ConvertDegreesToRadians(pointTemp.longitude);
			double latitudeRad = Sphere::ConvertDegreesToRadians(pointTemp.latitude);
			toAdd.AddPoint({
				(longitudeRad - misc_.min_lon) * zoomCoef + renderSettings.padding,
				(misc_.max_lat - latitudeRad) * zoomCoef + renderSettings.padding
			});
		}

		doc.Add(move(toAdd));

		counter++;
	}
}

void MapBuilder::PrintStopCircles(Svg::Document& doc) const {
	for (const auto& i : stops) {
		auto stopPos = i.second->position;
		double longitudeRad = Sphere::ConvertDegreesToRadians(stopPos.longitude);
		double latitudeRad = Sphere::ConvertDegreesToRadians(stopPos.latitude);
		double x = (longitudeRad - misc_.min_lon) * zoomCoef + renderSettings.padding;
		double y = (misc_.max_lat - latitudeRad) * zoomCoef + renderSettings.padding;
		doc.Add(
			Svg::Circle{}.
			SetFillColor("white").
			SetRadius(renderSettings.stop_radius).
			SetCenter({x, y})
		);
	}
}

void MapBuilder::PrintStopNames(Svg::Document& doc) const {
	for (const auto& i : stops) {
		auto stopPos = i.second->position;
		double longitudeRad = Sphere::ConvertDegreesToRadians(stopPos.longitude);
		double latitudeRad = Sphere::ConvertDegreesToRadians(stopPos.latitude);
		double x = (longitudeRad - misc_.min_lon) * zoomCoef + renderSettings.padding;
		double y = (misc_.max_lat - latitudeRad) * zoomCoef + renderSettings.padding;
		doc.Add( // подложка
			Svg::Text{}.
			SetPoint({ x, y }).
			SetOffset(renderSettings.stop_label_offset).
			SetFontSize(renderSettings.stop_label_font_size).
			SetFontFamily("Verdana").
			SetData(i.first).
			SetFillColor(renderSettings.underlayer_color).
			SetStrokeColor(renderSettings.underlayer_color).
			SetStrokeWidth(renderSettings.underlayer_width).
			SetStrokeLineCap("round").
			SetStrokeLineJoin("round")
			);

		doc.Add( // название
			Svg::Text{}.
			SetPoint({ x, y }).
			SetOffset(renderSettings.stop_label_offset).
			SetFontSize(renderSettings.stop_label_font_size).
			SetFontFamily("Verdana").
			SetData(i.first).
			SetFillColor("black")
			);
	}
}

std::string MapBuilder::RenderMap() const {
	return *map_;
}