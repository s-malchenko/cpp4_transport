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

void RequestParserTest_Info()
{

}
