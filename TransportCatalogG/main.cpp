#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"

#include <iostream>
#include <sstream>

using namespace std;

int main() {

    //istringstream ss(
    //    "{"
    //    "    \"routing_settings\": {"
    //    "        \"bus_wait_time\": 2,"
    //    "        \"bus_velocity\": 30"
    //    "    },"
    //    "    \"render_settings\": {"
    //    "        \"width\": 1200,"
    //    "        \"height\": 1200,"
    //    "        \"padding\": 50,"
    //    "        \"stop_radius\": 5,"
    //    "        \"line_width\": 14,"
    //    "        \"bus_label_font_size\": 20,"
    //    "        \"bus_label_offset\": ["
    //    "            7,"
    //    "            15"
    //    "        ],"
    //    "        \"stop_label_font_size\": 20,"
    //    "        \"stop_label_offset\": ["
    //    "            7,"
    //    "            -3"
    //    "        ],"
    //    "        \"underlayer_color\": ["
    //    "            255,"
    //    "            255,"
    //    "            255,"
    //    "            0.85"
    //    "        ],"
    //    "        \"underlayer_width\": 3,"
    //    "        \"color_palette\": ["
    //    "            \"green\","
    //    "            ["
    //    "                255,"
    //    "                160,"
    //    "                0"
    //    "            ],"
    //    "            \"red\""
    //    "        ],"
    //    "        \"layers\": ["
    //    "            \"bus_lines\","
    //    "            \"stop_points\","
    //    "            \"bus_labels\""
    //    "        ]"
    //    "    },"
    //    "    \"base_requests\": ["
    //    "        {"
    //    "            \"type\": \"Bus\","
    //    "            \"name\": \"14\","
    //    "            \"stops\": ["
    //    "                \"Ulica Lizy CHajkinoj\","
    //    "                \"Elektroseti\","
    //    "                \"Riv'erskij most\","
    //    "                \"Gostinica Sochi\","
    //    "                \"Kubanskaya ulica\","
    //    "                \"Po trebovaniyu\","
    //    "                \"Ulica Dokuchaeva\","
    //    "                \"Ulica Lizy CHajkinoj\""
    //    "            ],"
    //    "            \"is_roundtrip\": true"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Bus\","
    //    "            \"name\": \"24\","
    //    "            \"stops\": ["
    //    "                \"Ulica Dokuchaeva\","
    //    "                \"Parallel'naya ulica\","
    //    "                \"Elektroseti\","
    //    "                \"Sanatorij Rodina\""
    //    "            ],"
    //    "            \"is_roundtrip\": false"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Bus\","
    //    "            \"name\": \"114\","
    //    "            \"stops\": ["
    //    "                \"Morskoj vokzal\","
    //    "                \"Riv'erskij most\""
    //    "            ],"
    //    "            \"is_roundtrip\": false"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Ulica Lizy CHajkinoj\","
    //    "            \"latitude\": 43.590317,"
    //    "            \"longitude\": 39.746833,"
    //    "            \"road_distances\": {"
    //    "                \"Elektroseti\": 4300,"
    //    "                \"Ulica Dokuchaeva\": 2000"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Morskoj vokzal\","
    //    "            \"latitude\": 43.581969,"
    //    "            \"longitude\": 39.719848,"
    //    "            \"road_distances\": {"
    //    "                \"Riv'erskij most\": 850"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Elektroseti\","
    //    "            \"latitude\": 43.598701,"
    //    "            \"longitude\": 39.730623,"
    //    "            \"road_distances\": {"
    //    "                \"Sanatorij Rodina\": 4500,"
    //    "                \"Parallel'naya ulica\": 1200,"
    //    "                \"Riv'erskij most\": 1900"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Riv'erskij most\","
    //    "            \"latitude\": 43.587795,"
    //    "            \"longitude\": 39.716901,"
    //    "            \"road_distances\": {"
    //    "                \"Morskoj vokzal\": 850,"
    //    "                \"Gostinica Sochi\": 1740"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Gostinica Sochi\","
    //    "            \"latitude\": 43.578079,"
    //    "            \"longitude\": 39.728068,"
    //    "            \"road_distances\": {"
    //    "                \"Kubanskaya ulica\": 320"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Kubanskaya ulica\","
    //    "            \"latitude\": 43.578509,"
    //    "            \"longitude\": 39.730959,"
    //    "            \"road_distances\": {"
    //    "                \"Po trebovaniyu\": 370"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Po trebovaniyu\","
    //    "            \"latitude\": 43.579285,"
    //    "            \"longitude\": 39.733742,"
    //    "            \"road_distances\": {"
    //    "                \"Ulica Dokuchaeva\": 600"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Ulica Dokuchaeva\","
    //    "            \"latitude\": 43.585586,"
    //    "            \"longitude\": 39.733879,"
    //    "            \"road_distances\": {"
    //    "                \"Parallel'naya ulica\": 1100"
    //    "            }"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Parallel'naya ulica\","
    //    "            \"latitude\": 43.590041,"
    //    "            \"longitude\": 39.732886,"
    //    "            \"road_distances\": {}"
    //    "        },"
    //    "        {"
    //    "            \"type\": \"Stop\","
    //    "            \"name\": \"Sanatorij Rodina\","
    //    "            \"latitude\": 43.601202,"
    //    "            \"longitude\": 39.715498,"
    //    "            \"road_distances\": {}"
    //    "        }"
    //    "    ],"
    //    "    \"stat_requests\": ["
    //    "        {"
    //    "            \"id\": 826874078,"
    //    "            \"type\": \"Bus\","
    //    "            \"name\": \"14\""
    //    "        },"
    //    "        {"
    //    "            \"id\": 1086967114,"
    //    "            \"type\": \"Route\","
    //    "            \"from\": \"Morskoj vokzal\","
    //    "            \"to\": \"Parallel'naya ulica\""
    //    "        },"
    //    "        {"
    //    "            \"id\": 1218663236,"
    //    "            \"type\": \"Map\""
    //    "        }"
    //    "    ]"
    //    "}"
    //);

    const auto input_doc = Json::Load(cin);
    const auto& input_map = input_doc.GetRoot().AsMap();

    const TransportCatalog db(
        Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
        input_map.at("routing_settings").AsMap(),
        input_map.at("render_settings").AsMap()
        );

    Json::PrintValue(
        Requests::ProcessAll(db, input_map.at("stat_requests").AsArray()),
        cout
        );
    cout << endl;

    return 0;
}