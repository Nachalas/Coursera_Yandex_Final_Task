#pragma once

#include <map>

#include "svg_lib.h"
#include "descriptions.h"
#include "json.h"

class MapBuilder {
public:
	MapBuilder(
		const Descriptions::StopsDict& stopsDict,
		const Descriptions::BusesDict& busesDict,
		const Json::Dict& renderSetingsJSON,
		const Descriptions::Misc& misc
		);

	std::string RenderMap() const;
private:
	enum class Layers {
		BUS_LINES,
		BUS_LABELS,
		STOP_POINTS,
		STOP_LABELS
	};

	struct RenderSettings {
		double width = 0.;
		double height = 0.;
		double padding = 0.;
		double stop_radius = 0.;
		double line_width = 0.;
		int stop_label_font_size = 0;
		Svg::Point stop_label_offset = {0.,0.};
		Svg::Color underlayer_color = Svg::NoneColor;
		double underlayer_width = 0.;
		vector<Svg::Color> color_palette;
		int bus_label_font_size = 0;
		Svg::Point bus_label_offset = {0.,0.};
		vector<Layers> layers_;
	};

	RenderSettings MakeRenderSettings(const Json::Dict& json);
	vector<Layers> MakeLayers(const vector<Json::Node>& json);
	Svg::Color DeduceColor(const Json::Node& node);
	void PrintRoutes(Svg::Document& doc) const;
	void PrintStopCircles(Svg::Document& doc) const;
	void PrintStopNames(Svg::Document& doc) const;
	void PrintBusNames(Svg::Document& doc) const;
	double calculateTempZoomCoef(double w_or_h, double max, double min) const;
	double calculateZoomCoef() const;
	void PrintBusNameAtStop(const std::string& busName, const string& stopName, size_t colorCounter, Svg::Document& doc) const;
	void MakeMap() const;
	void PrintLayer(Layers layer, Svg::Document& doc) const;

	RenderSettings renderSettings;
	Descriptions::Misc misc_;
	Descriptions::BusesDictOrdered buses;
	Descriptions::StopsDictOrdered stops;
	double zoomCoef = 0.;
	mutable optional<string> map_;
};