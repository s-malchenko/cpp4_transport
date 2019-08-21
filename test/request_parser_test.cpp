#include "request_parser_test.h"
#include "request_parser.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <iostream>
#include <sstream>

using namespace std;

void RequestParserTest_Data()
{
    StandardParser parser;

    string str = "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya";
    auto request = parser.ParseDataRequest(str);
    ASSERT_EQUAL(RequestType::DATA, request->Type());
    ASSERT_EQUAL(RequestCmd::BUS, request->Cmd());
    ASSERT_EQUAL("256", request->Name());
    auto bus = static_cast<BusRequest *>(request.get());
    ASSERT_EQUAL(true, bus->Roundtrip());
    vector<string> stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya"};
    ASSERT_EQUAL(stops, bus->Stops());

    str = "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam";
    request = parser.ParseDataRequest(str);
    ASSERT_EQUAL(RequestType::DATA, request->Type());
    ASSERT_EQUAL(RequestCmd::STOP, request->Cmd());
    ASSERT_EQUAL("Biryulyovo Zapadnoye", request->Name());
    auto stop = static_cast<StopRequest *>(request.get());
    ASSERT(doublesEqual(stop->Site().latitude, 55.574371));
    ASSERT(doublesEqual(stop->Site().longitude, 37.6517));
    unordered_map<string, unsigned int> dists = {{"Rossoshanskaya ulitsa", 7500}, {"Biryusinka", 1800}, {"Universam", 2400}};
    ASSERT_EQUAL(stop->Distances(), dists);
}

void RequestParserTest_DataJson()
{
    JsonParser parser;
    string str = R"({
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
    })";
    auto request = parser.ParseDataRequest(str);
    ASSERT_EQUAL(RequestType::DATA, request->Type());
    ASSERT_EQUAL(RequestCmd::BUS, request->Cmd());
    ASSERT_EQUAL("256", request->Name());
    auto bus = static_cast<BusRequest *>(request.get());
    ASSERT_EQUAL(true, bus->Roundtrip());
    vector<string> stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"};
    ASSERT_EQUAL(bus->Stops(), stops);

    str = R"({
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 5600,
        "Biryulyovo Tovarnaya": 900
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    })";

    request = parser.ParseDataRequest(str);
    ASSERT_EQUAL(RequestType::DATA, request->Type());
    ASSERT_EQUAL(RequestCmd::STOP, request->Cmd());
    ASSERT_EQUAL("Universam", request->Name());
    auto stop = static_cast<StopRequest *>(request.get());
    ASSERT(doublesEqual(stop->Site().latitude, 55.587655));
    ASSERT(doublesEqual(stop->Site().longitude, 37.645687));
    unordered_map<string, unsigned int> dists = {{"Rossoshanskaya ulitsa", 5600}, {"Biryulyovo Tovarnaya", 900}};
    ASSERT_EQUAL(stop->Distances(), dists);
}
void RequestParserTest_InfoJson()
{
    JsonParser parser;
    string str = R"({
      "type": "Bus",
      "name": "751",
      "id": 194217464
    })";
    auto request = parser.ParseInfoRequest(str);
    ASSERT_EQUAL(RequestType::INFO, request->Type());
    ASSERT_EQUAL(RequestCmd::BUS, request->Cmd());
    ASSERT_EQUAL("751", request->Name());
    ASSERT_EQUAL(194217464u, request->Id());

    str = R"({
      "type": "Stop",
      "name": "Samara",
      "id": 746888088
    })";

    request = parser.ParseInfoRequest(str);
    ASSERT_EQUAL(RequestType::INFO, request->Type());
    ASSERT_EQUAL(RequestCmd::STOP, request->Cmd());
    ASSERT_EQUAL("Samara", request->Name());
    ASSERT_EQUAL(746888088u, request->Id());
}

void RequestParserTest_GetRequestsJson()
{
    stringstream ss(R"({
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
    vector<string> data =
    {
        R"({
      "type": "Stop",
      "road_distances": {
        "Marushkino": 3900
      },
      "longitude": 37.20829,
      "name": "Tolstopaltsevo",
      "latitude": 55.611087
    })",
        R"({
      "type": "Stop",
      "road_distances": {
        "Rasskazovka": 9900
      },
      "longitude": 37.209755,
      "name": "Marushkino",
      "latitude": 55.595884
    })",
    R"({
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
    })",
    R"({
      "type": "Bus",
      "name": "750",
      "stops": [
        "Tolstopaltsevo",
        "Marushkino",
        "Rasskazovka"
      ],
      "is_roundtrip": false
    })",
    R"({
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.333324,
      "name": "Rasskazovka",
      "latitude": 55.632761
    })",
    R"({
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 7500,
        "Biryusinka": 1800,
        "Universam": 2400
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    })",
    R"({
      "type": "Stop",
      "road_distances": {
        "Universam": 750
      },
      "longitude": 37.64839,
      "name": "Biryusinka",
      "latitude": 55.581065
    })",
    R"({
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 5600,
        "Biryulyovo Tovarnaya": 900
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    })",
    R"({
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Passazhirskaya": 1300
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    })",
    R"({
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Zapadnoye": 1200
      },
      "longitude": 37.659164,
      "name": "Biryulyovo Passazhirskaya",
      "latitude": 55.580999
    })",
    R"({
      "type": "Bus",
      "name": "828",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Universam",
        "Rossoshanskaya ulitsa",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    })",
    R"({
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.605757,
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579
    })",
    R"({
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.603831,
      "name": "Prazhskaya",
      "latitude": 55.611678
    })"};
  vector<string> info = {
    R"({
      "type": "Bus",
      "name": "256",
      "id": 1965312327
    })",
    R"({
      "type": "Bus",
      "name": "750",
      "id": 519139350
    })",
    R"({
      "type": "Bus",
      "name": "751",
      "id": 194217464
    })",
    R"({
      "type": "Stop",
      "name": "Samara",
      "id": 746888088
    })",
    R"({
      "type": "Stop",
      "name": "Prazhskaya",
      "id": 65100610
    })",
    R"({
      "type": "Stop",
      "name": "Biryulyovo Zapadnoye",
      "id": 1042838872
    })"};

    JsonParser parser;
    ASSERT_EQUAL(parser.GetDataRequests(ss), data);
    ASSERT_EQUAL(parser.GetInfoRequests(ss), info);
}
