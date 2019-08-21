#include "request_processor_test.h"
#include "request_processor.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <utility>
#include <iostream>

using namespace std;

static inline void addStopRequest(RequestProcessor &rp,
                                  const string &name,
                                  long double lat,
                                  long double lon,
                                  unordered_map<string, unsigned int> dist = {},
                                  unsigned int id = 0)
{
    auto ptr = make_unique<StopRequest>(name, id);
    ptr->Site({lat, lon}).Distances(dist);
    rp.AddRequest(move(ptr));
}

static inline void addBusRequest(RequestProcessor &rp,
                                 const string &name,
                                 bool round,
                                 vector<string_view> stops,
                                 unsigned int id = 0)
{
    auto ptr = make_unique<BusRequest>(name, id);
    ptr->Roundtrip(round).Stops(stops);
    rp.AddRequest(move(ptr));
}

static inline void addInfoRequest(RequestProcessor &rp,
                                  const string &name,
                                  bool stop = false,
                                  unsigned int id = 0)
{
    auto cmd = RequestCmd::BUS;

    if (stop)
    {
        cmd = RequestCmd::STOP;
    }

    rp.AddRequest(make_unique<InfoRequest>(cmd, name, id));
}

#define CHECK_BUS_RESPONSE(resp, err, name, stops, ustops, len, curv) \
{ \
    auto busPtr = static_cast<BusResponse *>(resp.get()); \
    ASSERT_EQUAL(busPtr->Name(), name); \
    ASSERT_EQUAL(busPtr->Error(), err); \
    if (!err) \
    { \
        ASSERT_EQUAL(busPtr->Stops(), stops); \
        ASSERT_EQUAL(busPtr->UniqueStops(), ustops); \
        ASSERT_EQUAL(busPtr->Length(), len); \
        ASSERT(doublesEqual(busPtr->Curvature(), curv)); \
    } \
}

#define CHECK_STOP_RESPONSE(resp, err, name, buses) \
{ \
    auto busPtr = static_cast<StopResponse *>(resp.get()); \
    ASSERT_EQUAL(busPtr->Name(), name); \
    ASSERT_EQUAL(busPtr->Error(), err); \
    if (!err) \
    { \
        ASSERT_EQUAL(busPtr->Buses(), buses); \
    } \
}

void RequestProcessorTest_Smoke()
{

    RequestProcessor rp;
    addStopRequest(rp, "Tolstopaltsevo", 55.611087, 37.20829, {{"Marushkino", 3900}});
    addStopRequest(rp, "Marushkino", 55.595884, 37.209755, {{"Rasskazovka", 9900}});
    addBusRequest(rp, "256", true,
        {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"});
    addBusRequest(rp, "750", false, {"Tolstopaltsevo", "Marushkino", "Rasskazovka"});
    addStopRequest(rp, "Rasskazovka", 55.632761, 37.333324);
    addStopRequest(rp, "Biryulyovo Zapadnoye", 55.574371, 37.6517,
        {{"Rossoshanskaya ulitsa", 7500}, {"Biryusinka", 1800}, {"Universam", 2400}});
    addStopRequest(rp, "Biryusinka", 55.581065, 37.64839, {{"Universam", 750}});
    addStopRequest(rp, "Universam", 55.587655, 37.645687, {{"Rossoshanskaya  ulitsa", 5600}, {"Biryulyovo Tovarnaya", 900}});
    addStopRequest(rp, "Biryulyovo Tovarnaya", 55.592028, 37.653656, {{"Biryulyovo Passazhirskaya", 1300}});
    addStopRequest(rp, "Biryulyovo Passazhirskaya", 55.580999, 37.659164, {{"Biryulyovo Zapadnoye", 1200}});
    addBusRequest(rp, "828", true,
        {"Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"});
    addStopRequest(rp, "Rossoshanskaya ulitsa", 55.595579, 37.605757);
    addStopRequest(rp, "Prazhskaya", 55.611678, 37.603831);
    addInfoRequest(rp, "256");
    addInfoRequest(rp, "750");
    addInfoRequest(rp, "751");
    addInfoRequest(rp, "Samara", true);
    addInfoRequest(rp, "Prazhskaya", true);
    addInfoRequest(rp, "Biryulyovo Zapadnoye", true);
    auto responses = rp.Proceed();
    CHECK_BUS_RESPONSE(responses[0], false, "256", 6u, 5u, 5950u, 1.361239);
    CHECK_BUS_RESPONSE(responses[1], false, "750", 5u, 3u, 27600u, 1.318084);
    CHECK_BUS_RESPONSE(responses[2], true, "751", 0u, 0u, 0u, 1);

    set<string_view> testBuses;
    CHECK_STOP_RESPONSE(responses[3], true, "Samara", testBuses);
    CHECK_STOP_RESPONSE(responses[4], false, "Prazhskaya", testBuses);
    testBuses = {"256", "828"};
    CHECK_STOP_RESPONSE(responses[5], false, "Biryulyovo Zapadnoye", testBuses);
}
