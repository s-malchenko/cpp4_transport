#include "request_processor_test.h"
#include "request_processor.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <utility>

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
                                 vector<string> stops,
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

static inline void addSettingsRequest(RequestProcessor &rp,
                                  unsigned int time,
                                  unsigned int velocity)
{
    rp.AddRequest(make_unique<SettingsRequest>(time, velocity));
}

static inline void addRouteRequest(RequestProcessor &rp,
                                  const string &from,
                                  const string &to,
                                  unsigned int id = 0)
{
    rp.AddRequest(make_unique<RouteRequest>(from, to, id));
}

#define CHECK_BUS_RESPONSE(resp, err, name, stops, ustops, len, curv, id) \
{ \
    auto busPtr = static_cast<BusResponse *>(resp.get()); \
    ASSERT_EQUAL(busPtr->Name(), name); \
    ASSERT_EQUAL(busPtr->Error(), err); \
    ASSERT_EQUAL(busPtr->Id(), id); \
    if (!err) \
    { \
        ASSERT_EQUAL(busPtr->Stops(), stops); \
        ASSERT_EQUAL(busPtr->UniqueStops(), ustops); \
        ASSERT_EQUAL(busPtr->Length(), len); \
        ASSERT(doublesEqual(busPtr->Curvature(), curv)); \
    } \
}

#define CHECK_STOP_RESPONSE(resp, err, name, buses, id) \
{ \
    auto busPtr = static_cast<StopResponse *>(resp.get()); \
    ASSERT_EQUAL(busPtr->Name(), name); \
    ASSERT_EQUAL(busPtr->Error(), err); \
    ASSERT_EQUAL(busPtr->Id(), id); \
    if (!err) \
    { \
        ASSERT_EQUAL(busPtr->Buses(), buses); \
    } \
}

#define ROUTE_ITEMS_EQUAL(lhs, rhs) \
{ \
    ASSERT_EQUAL(lhs->Type(), rhs->Type()); \
    ASSERT_EQUAL(lhs->Name(), rhs->Name()); \
    ASSERT(doublesEqual(lhs->Time(), rhs->Time())); \
    if (lhs->Type() == RouteItemType::RIDE) \
    { \
        ASSERT_EQUAL(reinterpret_cast<BusItem *>(lhs.get())->SpanCount(), reinterpret_cast<BusItem *>(rhs.get())->SpanCount()); \
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
    addInfoRequest(rp, "256", false, 25);
    addInfoRequest(rp, "750", false, 322);
    addInfoRequest(rp, "751", false, 789789);
    addInfoRequest(rp, "Samara", true, 44);
    addInfoRequest(rp, "Prazhskaya", true, 685u);
    addInfoRequest(rp, "Biryulyovo Zapadnoye", true, 65542u);
    auto responses = rp.Proceed();
    CHECK_BUS_RESPONSE(responses[0], false, "256", 6u, 5u, 5950u, 1.361239, 25u);
    CHECK_BUS_RESPONSE(responses[1], false, "750", 5u, 3u, 27600u, 1.318084, 322u);
    CHECK_BUS_RESPONSE(responses[2], true, "751", 0u, 0u, 0u, 1, 789789u);

    set<string> testBuses;
    CHECK_STOP_RESPONSE(responses[3], true, "Samara", testBuses, 44u);
    CHECK_STOP_RESPONSE(responses[4], false, "Prazhskaya", testBuses, 685u);
    testBuses = {"256", "828"};
    CHECK_STOP_RESPONSE(responses[5], false, "Biryulyovo Zapadnoye", testBuses, 65542u);
}

void RequestProcessorTest_SimpleRoutes()
{
    RequestProcessor rp;
    addSettingsRequest(rp, 14, 20);

    addStopRequest(rp, "stop1", 55.611087, 37.20829, {{"stop2", 500}});
    addStopRequest(rp, "stop2", 55.595884, 37.209755, {{"stop3", 100}});
    addStopRequest(rp, "stop3", 55.632761, 37.333324, {{"stop2", 300}});
    addBusRequest(rp, "256", false, {"stop1", "stop2", "stop3"});

    addStopRequest(rp, "stop4", 55.581065, 37.64839, {{"stop5", 1000}});
    addStopRequest(rp, "stop5", 55.587655, 37.645687, {{"stop6", 800}});
    addStopRequest(rp, "stop6", 55.592028, 37.653656, {{"stop4", 1240}});
    addBusRequest(rp, "ring", true, {"stop4", "stop5", "stop6"});

    addRouteRequest(rp, "stop1", "stop3", 56982);
    addRouteRequest(rp, "stop3", "stop2", 6666);
    addRouteRequest(rp, "stop4", "stop6", 9989);
    addRouteRequest(rp, "stop6", "stop5", 3);
    addRouteRequest(rp, "stop3", "stop4", 54);

    auto responses = rp.Proceed();
    ASSERT_EQUAL(responses.size(), 5u);

    // stop1 to stop3
    ASSERT((bool)responses[0]);
    auto resp = static_cast<RouteResponse *>(responses[0].get());
    ASSERT_EQUAL(resp->Id(), 56982u);
    ASSERT(doublesEqual(resp->Route()->TotalTime(), 15.8));
    auto items = resp->Route()->Items();
    ASSERT_EQUAL(items.size(), 2u);
    ROUTE_ITEMS_EQUAL(items[0], make_unique<WaitItem>(14, "stop1"));
    ROUTE_ITEMS_EQUAL(items[1], make_unique<BusItem>(1.8, "256", 2));

    // stop3 to stop2
    ASSERT((bool)responses[1]);
    resp = static_cast<RouteResponse *>(responses[1].get());
    ASSERT_EQUAL(resp->Id(), 6666u);
    ASSERT(doublesEqual(resp->Route()->TotalTime(), 14.9));
    items = resp->Route()->Items();
    ASSERT_EQUAL(items.size(), 2u);
    ROUTE_ITEMS_EQUAL(items[0], make_unique<WaitItem>(14, "stop3"));
    ROUTE_ITEMS_EQUAL(items[1], make_unique<BusItem>(0.9, "256", 1));

    // stop4 to stop6
    ASSERT((bool)responses[2]);
    resp = static_cast<RouteResponse *>(responses[2].get());
    ASSERT_EQUAL(resp->Id(), 9989u);
    ASSERT(doublesEqual(resp->Route()->TotalTime(), 19.4));
    items = resp->Route()->Items();
    ASSERT_EQUAL(items.size(), 2u);
    ROUTE_ITEMS_EQUAL(items[0], make_unique<WaitItem>(14, "stop4"));
    ROUTE_ITEMS_EQUAL(items[1], make_unique<BusItem>(5.4, "ring", 2));

    // stop6 to stop5
    ASSERT((bool)responses[3]);
    resp = static_cast<RouteResponse *>(responses[3].get());
    ASSERT_EQUAL(resp->Id(), 3u);
    ASSERT(doublesEqual(resp->Route()->TotalTime(), 34.72));
    items = resp->Route()->Items();
    ASSERT_EQUAL(items.size(), 4u);
    ROUTE_ITEMS_EQUAL(items[0], make_unique<WaitItem>(14, "stop6"));
    ROUTE_ITEMS_EQUAL(items[1], make_unique<BusItem>(3.72, "ring", 1));
    ROUTE_ITEMS_EQUAL(items[2], make_unique<WaitItem>(14, "stop4"));
    ROUTE_ITEMS_EQUAL(items[3], make_unique<BusItem>(3, "ring", 1));

    // stop3 to stop4, no route
    ASSERT((bool)responses[4]);
    ASSERT(responses[4]->Error());
}