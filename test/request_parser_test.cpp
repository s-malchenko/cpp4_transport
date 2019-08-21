#include "request_parser_test.h"
#include "request_parser.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <iostream>

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

