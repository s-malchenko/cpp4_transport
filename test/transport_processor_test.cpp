#include "transport_processor_test.h"
#include "transport_processor.h"
#include "test_runner.h"
#include "json_compare.h"

#include <sstream>
#include <fstream>
#include <string>

using namespace std;

#define TEST_RUN(in, expected) \
{ \
    auto parser = make_shared<StandardParser>(); \
    stringstream out; \
    stringstream input(in); \
    RunTransportProcessor(parser, input, out); \
    ASSERT_EQUAL(out.str(), expected); \
}

void TransportProcessorTest_Run()
{
    string inStr = R"(13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye
)";
    string outStr = R"(Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 curvature
Bus 750: 5 stops on route, 3 unique stops, 27600 route length, 1.318084 curvature
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
)";
    TEST_RUN(inStr, outStr);
}

#define TEST_RUN_JSON(in, expected) \
{ \
    auto parser = make_shared<JsonParser>(); \
    stringstream out; \
    RunTransportProcessor(parser, in, out); \
    ofstream debug("debug.json"); \
    debug << out.str(); \
    ASSERT(JsonCompare(out, expected)); \
}

void TransportProcessorTest_RunJson()
{
    stringstream inStr(R"({
  "base_requests": [
    {
      "type": "Stop",
      "road_distances": {
        "Marushkino": 3900
      },
      "longitude": 37.20829,
      "name": "Tolstopaltsevo",
      "latitude": 55.611087
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rasskazovka": 9900
      },
      "longitude": 37.209755,
      "name": "Marushkino",
      "latitude": 55.595884
    },
    {
      "type": "Bus",
      "name": "256",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Biryusinka",
        "Universam",
        "Biryulyovo Tovarnaya",
        "Biryulyovo Passazhirskaya",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "750",
      "stops": [
        "Tolstopaltsevo",
        "Marushkino",
        "Rasskazovka"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.333324,
      "name": "Rasskazovka",
      "latitude": 55.632761
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 7500,
        "Biryusinka": 1800,
        "Universam": 2400
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    },
    {
      "type": "Stop",
      "road_distances": {
        "Universam": 750
      },
      "longitude": 37.64839,
      "name": "Biryusinka",
      "latitude": 55.581065
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 5600,
        "Biryulyovo Tovarnaya": 900
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Passazhirskaya": 1300
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Zapadnoye": 1200
      },
      "longitude": 37.659164,
      "name": "Biryulyovo Passazhirskaya",
      "latitude": 55.580999
    },
    {
      "type": "Bus",
      "name": "828",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Universam",
        "Rossoshanskaya ulitsa",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.605757,
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.603831,
      "name": "Prazhskaya",
      "latitude": 55.611678
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "256",
      "id": 1965312327
    },
    {
      "type": "Bus",
      "name": "750",
      "id": 519139350
    },
    {
      "type": "Bus",
      "name": "751",
      "id": 194217464
    },
    {
      "type": "Stop",
      "name": "Samara",
      "id": 746888088
    },
    {
      "type": "Stop",
      "name": "Prazhskaya",
      "id": 65100610
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Zapadnoye",
      "id": 1042838872
    }
  ]
})");
    stringstream outStr(R"([
{ "request_id": 1965312327, "route_length": 5950, "curvature": 1.36124, "stop_count": 6, "unique_stop_count": 5 },
{ "request_id": 519139350, "route_length": 27600, "curvature": 1.31808, "stop_count": 5, "unique_stop_count": 3 },
{ "request_id": 194217464, "error_message": "not found" },
{ "request_id": 746888088, "error_message": "not found" },
{ "request_id": 65100610, "buses": [] },
{ "request_id": 1042838872, "buses": ["256", "828"] }
])");
    TEST_RUN_JSON(inStr, outStr);

    inStr = stringstream(R"({"base_requests": [{"type": "Stop", "name": "Tolstopaltsevo", "latitude": 55.611087, "longitude": 37.20829, "road_distances": {"Marushkino": 3900}}, {"type": "Stop", "name": "Marushkino", "latitude": 55.595884, "longitude": 37.209755, "road_distances": {"Rasskazovka": 9900}}, {"type": "Bus", "name": "256", "stops": ["Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Bus", "name": "750", "stops": ["Tolstopaltsevo", "Marushkino", "Rasskazovka"], "is_roundtrip": false}, {"type": "Stop", "name": "Rasskazovka", "latitude": 55.632761, "longitude": 37.333324, "road_distances": {}}, {"type": "Stop", "name": "Biryulyovo Zapadnoye", "latitude": 55.574371, "longitude": 37.6517, "road_distances": {"Biryusinka": 1800, "Universam": 2400, "Rossoshanskaya ulitsa": 7500}}, {"type": "Stop", "name": "Biryusinka", "latitude": 55.581065, "longitude": 37.64839, "road_distances": {"Universam": 750}}, {"type": "Stop", "name": "Universam", "latitude": 55.587655, "longitude": 37.645687, "road_distances": {"Biryulyovo Tovarnaya": 900, "Rossoshanskaya ulitsa": 5600}}, {"type": "Stop", "name": "Biryulyovo Tovarnaya", "latitude": 55.592028, "longitude": 37.653656, "road_distances": {"Biryulyovo Passazhirskaya": 1300}}, {"type": "Stop", "name": "Biryulyovo Passazhirskaya", "latitude": 55.580999, "longitude": 37.659164, "road_distances": {"Biryulyovo Zapadnoye": 1200}}, {"type": "Bus", "name": "828", "stops": ["Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Stop", "name": "Rossoshanskaya ulitsa", "latitude": 55.595579, "longitude": 37.605757, "road_distances": {}}, {"type": "Stop", "name": "Prazhskaya", "latitude": 55.611678, "longitude": 37.603831, "road_distances": {}}], "stat_requests": [{"id": 1247206578, "type": "Bus", "name": "256"}, {"id": 1132150366, "type": "Bus", "name": "750"}, {"id": 2036773136, "type": "Bus", "name": "751"}, {"id": 1522107761, "type": "Stop", "name": "Samara"}, {"id": 122335467, "type": "Stop", "name": "Prazhskaya"}, {"id": 1193762553, "type": "Stop", "name": "Biryulyovo Zapadnoye"}]})");
    outStr = stringstream(R"([
    { "request_id": 1247206578, "route_length": 5950, "curvature": 1.36124, "stop_count": 6, "unique_stop_count": 5 },
    { "request_id": 1132150366, "route_length": 27600, "curvature": 1.31808, "stop_count": 5, "unique_stop_count": 3, },
    { "request_id": 2036773136, "error_message": "not found" },
    { "request_id": 1522107761, "error_message": "not found" },
    { "request_id": 122335467, "buses": [] },
    { "request_id": 1193762553, "buses": ["256", "828"]}
])");

    // some difficult parsing test
    inStr = stringstream(R"({"routing_settings": {"bus_wait_time": 6, "bus_velocity": 40},"base_requests": [{"type": "Bus", "name": "297", "stops": ["Biryulyovo Zapadnoye", "Biryulyovo Tovarnaya", "Universam", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Bus", "name": "635", "stops": ["Biryulyovo Tovarnaya", "Universam", "Prazhskaya"], "is_roundtrip": false}, {"type": "Stop", "name": "Biryulyovo Zapadnoye", "latitude": 55.574371, "longitude": 37.6517, "road_distances": {"Biryulyovo Tovarnaya": 2600}}, {"type": "Stop", "name": "Universam", "latitude": 55.587655, "longitude": 37.645687, "road_distances": {"Biryulyovo Tovarnaya": 1380, "Biryulyovo Zapadnoye": 2500, "Prazhskaya": 4650}}, {"type": "Stop", "name": "Biryulyovo Tovarnaya", "latitude": 55.592028, "longitude": 37.653656, "road_distances": {"Universam": 890}}, {"type": "Stop", "name": "Prazhskaya", "latitude": 55.611717, "longitude": 37.603938, "road_distances": {}}], "stat_requests": [{"id": 847104961, "type": "Bus", "name": "297"}, {"id": 1509410151, "type": "Bus", "name": "635"}, {"id": 1910120855, "type": "Stop", "name": "Universam"}, {"id": 1152333432, "type": "Route", "from": "Biryulyovo Zapadnoye", "to": "Universam"}, {"id": 744653477, "type": "Route", "from": "Biryulyovo Zapadnoye", "to": "Prazhskaya"}]})");
    stringstream out;
    auto parser = make_shared<JsonParser>();
    RunTransportProcessor(parser, inStr, out);
}

#define TEST_RUN_JSON_FILES(name1, name2) \
{ \
    auto inFile = ifstream(name1); \
    ASSERT((bool)inFile); \
    auto outFile = ifstream(name2); \
    ASSERT((bool)outFile); \
    TEST_RUN_JSON(inFile, outFile); \
}

void TransportProcessorTest_RunJsonExamples()
{
    TEST_RUN_JSON_FILES("../test/graph_test/in1.json", "../test/graph_test/out1.json");
    TEST_RUN_JSON_FILES("../test/graph_test/in2.json", "../test/graph_test/out2.json");
    TEST_RUN_JSON_FILES("../test/graph_test/in5.json", "../test/graph_test/out5.json");
    TEST_RUN_JSON_FILES("../test/graph_test/in3.json", "../test/graph_test/out3.json");
    TEST_RUN_JSON_FILES("../test/graph_test/in4.json", "../test/graph_test/out4.json");
}

