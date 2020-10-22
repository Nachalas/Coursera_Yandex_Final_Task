#pragma once

#include "json.h"
#include "sphere.h"

#include <string>
#include <unordered_map>
#include <variant>
#include <map>
#include <vector>

namespace Descriptions {
    struct Stop {
        std::string name;
        Sphere::Point position;
        std::unordered_map<std::string, int> distances;

        static Stop ParseFrom(const Json::Dict& attrs);
    };

    int ComputeStopsDistance(const Stop& lhs, const Stop& rhs);

    std::vector<std::string> ParseStops(const std::vector<Json::Node>& stop_nodes, bool is_roundtrip);

    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool isCircular = true;

        static Bus ParseFrom(const Json::Dict& attrs);
    };

    using InputQuery = std::variant<Stop, Bus>;

    std::vector<InputQuery> ReadDescriptions(const std::vector<Json::Node>& nodes);

    struct Misc {
        double min_lat = 0.;
        double max_lat = 0.;
        double min_lon = 0.;
        double max_lon = 0.;
    };

    template <typename Object>
    using Dict = std::unordered_map<std::string, const Object*>;

    using StopsDict = Dict<Stop>;
    using BusesDict = Dict<Bus>;

    template<typename Object>
    using DictOrdered = std::map<std::string, const Object*>;

    using StopsDictOrdered = DictOrdered<Stop>;
    using BusesDictOrdered = DictOrdered<Bus>;
}