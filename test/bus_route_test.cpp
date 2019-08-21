#include "bus_route_test.h"
#include "bus_route.h"
#include "bus_stop.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <unordered_map>
#include <sstream>

using namespace std;

class BusRouteTest : public BusRoute
{
public:
    BusRouteTest(const string &num, bool ring = false) : BusRoute(num, ring) {}
    bool GetRing() const { return _ring; }
    const optional<Distance> &DistanceOpt() const { return _distance; }
};

void BusRouteTest_Constructor()
{
    {
        string num = "78";
        BusRouteTest bus(num);
        ASSERT_EQUAL(num, bus.GetNumber());
        ASSERT_EQUAL(false, bus.GetRing());
        ASSERT(!bus.DistanceOpt());
    }
    {
        string num = "653";
        BusRouteTest bus(num, false);
        ASSERT_EQUAL(num, bus.GetNumber());
        ASSERT_EQUAL(false, bus.GetRing());
        ASSERT(!bus.DistanceOpt());
    }
    {
        string num = "777";
        BusRouteTest bus(num, true);
        ASSERT_EQUAL(num, bus.GetNumber());
        ASSERT_EQUAL(true, bus.GetRing());
        ASSERT(!bus.DistanceOpt());
    }
}

static inline void addStopToBase(unordered_map<string, BusStop> &base,
                                 const string &name,
                                 double lat, double lon)
{
    base[name] = {name, {lat, lon}};
}

#define TEST_GET_DISTANCE(stops, ring, base, expected) \
{ \
    BusRouteTest bus("1", ring); \
    bus.AssignStops(stops.begin(), stops.end()); \
    ASSERT_EQUAL(bus.GetDistance(base), expected); \
}

void BusRouteTest_GetDistanceStraight()
{
    unordered_map<string, BusStop> base;
    addStopToBase(base, "stop1", 0, 0);
    addStopToBase(base, "stop2", 45, 0);
    addStopToBase(base, "Tolstopaltsevo", 55.611087, 37.20829);
    addStopToBase(base, "Marushkino", 55.595884, 37.209755);
    addStopToBase(base, "Rasskazovka", 55.632761, 37.333324);
    addStopToBase(base, "Biryulyovo Zapadnoye", 55.574371, 37.6517);
    addStopToBase(base, "Biryusinka", 55.581065, 37.64839);
    addStopToBase(base, "Universam", 55.587655, 37.645687);
    addStopToBase(base, "Biryulyovo Tovarnaya", 55.592028, 37.653656);
    addStopToBase(base, "Biryulyovo Passazhirskaya", 55.580999, 37.659164);

    vector<string_view> stops = {"stop1", "stop2", "stop1", "stop2"};
    TEST_GET_DISTANCE(stops, false, base, PI * EARTH_RADIUS * 3 / 2);

    stops = {"stop1", "stop2", "stop1", "stop2", "stop1"};
    TEST_GET_DISTANCE(stops, true, base, PI * EARTH_RADIUS);

    stops = {"stop1", "stop2", "stop1", "stop2"};
    TEST_GET_DISTANCE(stops, true, base, PI * EARTH_RADIUS);

    stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam",
             "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya",
             "Biryulyovo Zapadnoye"
            };
    TEST_GET_DISTANCE(stops, true, base, 4371.02);
}

static inline void addTableEntry(DistanceTable &table, string_view s1, string_view s2, unsigned int lenTo, unsigned int lenFrom)
{
    table[string(s1)][string(s2)] = lenTo;
    table[string(s2)][string(s1)] = lenFrom;
}

void BusRouteTest_GetDistance()
{
    DistanceTable distances;
    addTableEntry(distances, "stop1", "stop2", 200, 100);
    addTableEntry(distances, "stop2", "stop3", 300, 150);
    addTableEntry(distances, "stop3", "stop1", 5000, 60);
    distances["stop4"]["stop1"] = 50;
    distances["stop4"]["stop2"] = 250;
    distances["stop4"]["stop3"] = 350;
    StopsTable stopsBase;
    addStopToBase(stopsBase, "stop1", 0, 0);
    addStopToBase(stopsBase, "stop2", 0, 0);
    addStopToBase(stopsBase, "stop3", 0, 0);
    addStopToBase(stopsBase, "stop4", 0, 0);
    AssignDistances(distances, stopsBase);

    vector<string_view> stops = {"stop1", "stop2", "stop1", "stop2"};
    TEST_GET_DISTANCE(stops, false, stopsBase, 900u);

    stops = {"stop1", "stop2", "stop3", "stop1"};
    TEST_GET_DISTANCE(stops, true, stopsBase, 5500u);

    stops = {"stop1", "stop2", "stop3"};
    TEST_GET_DISTANCE(stops, false, stopsBase, 750u);

    stops = {"stop1", "stop3"};
    TEST_GET_DISTANCE(stops, true, stopsBase, 5060u);

    stops = {"stop1", "stop3"};
    TEST_GET_DISTANCE(stops, false, stopsBase, 5060u);

    stops = {"stop1", "stop4", "stop3"};
    TEST_GET_DISTANCE(stops, false, stopsBase, 800u);
}

void BusRouteTest_GetCurvature()
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

    DistanceTable distances;
    distances["Biryulyovo Zapadnoye"]["Biryusinka"] = 1800;
    distances["Biryulyovo Zapadnoye"]["Universam"] = 2400;
    distances["Biryusinka"]["Universam"] = 750;
    distances["Universam"]["Biryulyovo Tovarnaya"] = 900;
    distances["Biryulyovo Tovarnaya"]["Biryulyovo Passazhirskaya"] = 1300;
    distances["Biryulyovo Passazhirskaya"]["Biryulyovo Zapadnoye"] = 1200;
    StopsTable base;
    addStopToBase(base, "Biryulyovo Zapadnoye", 55.574371, 37.6517);
    addStopToBase(base, "Biryusinka", 55.581065, 37.64839);
    addStopToBase(base, "Universam", 55.587655, 37.645687);
    addStopToBase(base, "Biryulyovo Tovarnaya", 55.592028, 37.653656);
    addStopToBase(base, "Biryulyovo Passazhirskaya", 55.580999, 37.659164);
    AssignDistances(distances, base);

    vector<string_view> stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"};
    BusRouteTest bus("1", true);
    bus.AssignStops(stops);
    ASSERT(doublesEqual(bus.GetCurvature(base), 1.361239));
}

void BusRouteTest_GetUniqueStopsCount()
{
    BusRouteTest busRing2("3", true);
    vector<string_view> stops = {"stop 1", "stop 2", "stop 1", "stop 2"};
    ASSERT_EQUAL(busRing2.GetUniqueStopsCount(), 0u);
    busRing2.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busRing2.GetUniqueStopsCount(), 2u);
}

void BusRouteTest_GetStopsCount()
{
    vector<string_view> stops = {"stop 1", "stop 2", "stop 1", "stop 2"};

    BusRouteTest busNotRing("1");
    ASSERT_EQUAL(busNotRing.GetStopsCount(), 0u);
    busNotRing.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busNotRing.GetStopsCount(), 7u);

    BusRouteTest busRing2("3", true);
    busRing2.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busRing2.GetStopsCount(), 5u);
}

#define CHECK_BUSES_FOR_STOP(base, stop, buses) \
{ \
    ASSERT_EQUAL(base[stop].GetBuses(), buses); \
}

void BusRouteTest_FillStopsInfo()
{
    unordered_map<string, BusStop> base;
    addStopToBase(base, "stop1", 0, 0);
    addStopToBase(base, "stop2", 45, 0);
    addStopToBase(base, "Tolstopaltsevo", 55.611087, 37.20829);
    addStopToBase(base, "Marushkino", 55.595884, 37.209755);
    addStopToBase(base, "Rasskazovka", 55.632761, 37.333324);
    addStopToBase(base, "Biryulyovo Zapadnoye", 55.574371, 37.6517);
    addStopToBase(base, "Biryusinka", 55.581065, 37.64839);
    addStopToBase(base, "Universam", 55.587655, 37.645687);
    addStopToBase(base, "Biryulyovo Tovarnaya", 55.592028, 37.653656);
    addStopToBase(base, "Biryulyovo Passazhirskaya", 55.580999, 37.659164);
    addStopToBase(base, "Biryulyovo 2", 55.587999, 37.659164);

    vector<BusRouteTest> testBuses;
    vector<string_view> stops = {"stop1", "stop2", "stop1", "stop2"};

    stops = {"stop1", "stop2", "stop1", "stop2", "stop1"};
    testBuses.push_back(string("1"));
    testBuses.back().AssignStops(stops);

    stops = {"stop1", "stop2", "stop1", "stop2"};
    testBuses.push_back(string("2"));
    testBuses.back().AssignStops(stops);

    stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam",
             "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya",
             "Biryulyovo Zapadnoye"
            };

    testBuses.push_back({string("33"), true});
    testBuses.back().AssignStops(stops);

    stops = {"Biryusinka", "stop2",
             "Biryulyovo Tovarnaya",
             "Biryulyovo Zapadnoye"
            };

    testBuses.push_back({string("44"), true});
    testBuses.back().AssignStops(stops);

    for (const auto &i : testBuses)
    {
        i.FillStopsInfo(base);
    }

    set<string_view> expected = {"1", "2"};
    CHECK_BUSES_FOR_STOP(base, "stop1", expected);
    expected = {"1", "2", "44"};
    CHECK_BUSES_FOR_STOP(base, "stop2", expected);
    expected = {"33", "44"};
    CHECK_BUSES_FOR_STOP(base, "Biryulyovo Tovarnaya", expected);
    expected = {};
    CHECK_BUSES_FOR_STOP(base, "Biryulyovo 2", expected);
}
