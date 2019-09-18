#include "transport_response_test.h"
#include "transport_response.h"
#include "test_runner.h"
#include "json_compare.h"
#include <sstream>

using namespace std;

#define TEST_PROCEED_RESPONSE(resp, result) \
{ \
    stringstream out; \
    resp.Proceed(out); \
    ASSERT_EQUAL(out.str(), result); \
}

#define TEST_PROCEED_RESPONSE_JSON(resp, result) \
{ \
    stringstream out; \
    stringstream resultStream(result); \
    resp.ProceedJSON(out); \
    ASSERT(JsonCompare(out, resultStream)); \
}

void TransportResponseTest_BusProceed()
{
    auto response = BusResponse("654", 0, true);
    TEST_PROCEED_RESPONSE(response,
                          "Bus 654: not found\n");

    response = BusResponse("33").Length(5658).Curvature(1.56).Stops(89).UniqueStops(56);
    TEST_PROCEED_RESPONSE(response,
                          "Bus 33: 89 stops on route, 56 unique stops, 5658 route length, 1.56 curvature\n");

    response = BusResponse("256").Length(5950).Curvature(1.361239).Stops(6).UniqueStops(5);
    TEST_PROCEED_RESPONSE(response,
                          "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 curvature\n");
}

void TransportResponseTest_StopProceed()
{
    auto response = StopResponse("654", 0, true);
    TEST_PROCEED_RESPONSE(response,
                          "Stop 654: not found\n");

    response = StopResponse("99");
    TEST_PROCEED_RESPONSE(response,
                          "Stop 99: no buses\n");

    set<string> buses = {"bus3", "bus1", "the coolest bus"};
    response = StopResponse("Pyater ochka").Buses(buses);
    TEST_PROCEED_RESPONSE(response,
                          "Stop Pyater ochka: buses bus1 bus3 the coolest bus\n");
}

void TransportResponseTest_BusProceedJson()
{
    auto response = BusResponse("654", 68, true);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({ "request_id": 68, "error_message": "not found" })");

    response = BusResponse("33", 88525).Length(5658).Curvature(1.56).Stops(89).UniqueStops(56);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({ "request_id": 88525, "route_length": 5658, "curvature": 1.56, "stop_count": 89, "unique_stop_count": 56 })");

    response = BusResponse("256", 65465465).Length(5950).Curvature(1.361239).Stops(6).UniqueStops(5);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({ "request_id": 65465465, "route_length": 5950, "curvature": 1.361239, "stop_count": 6, "unique_stop_count": 5 })");
}

void TransportResponseTest_StopProceedJson()
{
    auto response = StopResponse("654", 65985, true);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({ "request_id": 65985, "error_message": "not found" })");

    response = StopResponse("99", 122335467);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({ "request_id": 122335467, "buses": [] })");

    set<string> buses = {"bus3", "bus1", "the coolest bus"};
    response = StopResponse("Pyater ochka", 1193762553).Buses(buses);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({ "request_id": 1193762553, "buses": ["bus1", "bus3", "the coolest bus"]})");
}

void TransportResponseTest_RouteProceedJson()
{
    auto response = RouteResponse(4);
    auto route = make_shared<ComplexRoute>();

    response.Route(route);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({
        "error_message": "not found",
        "request_id": 4
    })");

    route->Valid(true);
    response.Route(route);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({
        "total_time": 0,
        "items": [],
        "request_id": 4
    })");
    route->AddItem(make_shared<WaitItem>(6, "Biryulyovo Zapadnoye"));
    route->AddItem(make_shared<BusItem>(5.235, "297", 2));

    response.Route(route);
    TEST_PROCEED_RESPONSE_JSON(response,
                               R"({
        "total_time": 11.235,
        "items": [
            {
                "time": 6.0,
                "type": "Wait",
                "stop_name": "Biryulyovo Zapadnoye"
            },
            {
                "span_count": 2,
                "bus": "297",
                "type": "Bus",
                "time": 5.235
            }
        ],
        "request_id": 4
    })");
}
