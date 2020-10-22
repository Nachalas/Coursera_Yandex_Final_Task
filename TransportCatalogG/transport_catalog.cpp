#include "transport_catalog.h"

#include <sstream>

using namespace std;

TransportCatalog::TransportCatalog(
    vector<Descriptions::InputQuery> data, 
    const Json::Dict& routing_settings_json,
    const Json::Dict& render_settings_json
    ) {
    auto stops_end = partition(begin(data), end(data), [](const auto& item) {
        return holds_alternative<Descriptions::Stop>(item);
        });

    double min_lat = 3.1415926535; //
    double max_lat = -3.1415926535; //
    double min_lon = 3.1415926535; // 
    double max_lon = -3.1415926535; // 
    Descriptions::StopsDict stops_dict;
    for (const auto& item : Range{ begin(data), stops_end }) {
        const auto& stop = get<Descriptions::Stop>(item);
        Sphere::Point temp = Sphere::Point::FromDegrees( //
            stop.position.latitude,
            stop.position.longitude
        );
        if (temp.latitude < min_lat) {
            min_lat = temp.latitude;
        }
        if (temp.latitude > max_lat) {
            max_lat = temp.latitude;
        }
        if (temp.longitude < min_lon) {
            min_lon = temp.longitude;
        }                                                
        if (temp.longitude > max_lon) {
            max_lon = temp.longitude;
        }
        stops_dict[stop.name] = &stop;
        stops_.insert({ stop.name, {} });
    }

    Descriptions::BusesDict buses_dict;
    for (const auto& item : Range{ stops_end, end(data) }) {
        const auto& bus = get<Descriptions::Bus>(item);

        buses_dict[bus.name] = &bus;
        buses_[bus.name] = Bus{
          bus.stops.size(),
          ComputeUniqueItemsCount(AsRange(bus.stops)),
          ComputeRoadRouteLength(bus.stops, stops_dict),
          ComputeGeoRouteDistance(bus.stops, stops_dict)
        };

        for (const string& stop_name : bus.stops) {
            stops_.at(stop_name).bus_names.insert(bus.name);
        }
    }

    router_ = make_unique<TransportRouter>(stops_dict, buses_dict, routing_settings_json);
    builder_ = make_unique<MapBuilder>(
        stops_dict, buses_dict,
        render_settings_json,
        Descriptions::Misc{ min_lat, max_lat, min_lon, max_lon }
    );
}

std::string TransportCatalog::RenderMap() const {
    return builder_->RenderMap();
}

const TransportCatalog::Stop* TransportCatalog::GetStop(const string& name) const {
    return GetValuePointer(stops_, name);
}

const TransportCatalog::Bus* TransportCatalog::GetBus(const string& name) const {
    return GetValuePointer(buses_, name);
}

optional<TransportRouter::RouteInfo> TransportCatalog::FindRoute(const string& stop_from, const string& stop_to) const {
    return router_->FindRoute(stop_from, stop_to);
}

int TransportCatalog::ComputeRoadRouteLength(
    const vector<string>& stops,
    const Descriptions::StopsDict& stops_dict
    ) {
    int result = 0;
    for (size_t i = 1; i < stops.size(); ++i) {
        result += Descriptions::ComputeStopsDistance(*stops_dict.at(stops[i - 1]), *stops_dict.at(stops[i]));
    }
    return result;
}

double TransportCatalog::ComputeGeoRouteDistance(
    const vector<string>& stops,
    const Descriptions::StopsDict& stops_dict
    ) {
    double result = 0;
    for (size_t i = 1; i < stops.size(); ++i) {
        result += Sphere::Distance(
            stops_dict.at(stops[i - 1])->position, stops_dict.at(stops[i])->position
            );
    }
    return result;
}