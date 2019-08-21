#include "transport_test.h"
#include "test_runner.h"
#include "input_parser_test.h"
#include "bus_stop_test.h"
#include "bus_route_test.h"
#include "request_parser_test.h"
#include "request_processor_test.h"
#include "transport_processor_test.h"
#include "transport_response_test.h"

void TestTransport()
{
    TestRunner tr;
    RUN_TEST(tr, InputParserTest_GetPart);
    RUN_TEST(tr, InputParserTest_ParseStops);
    RUN_TEST(tr, InputParserTest_ParseCoordinates);
    RUN_TEST(tr, InputParserTest_ParseStopDistances);
    RUN_TEST(tr, BusStopTest_Distance);
    RUN_TEST(tr, BusStopTest_Buses);
    RUN_TEST(tr, BusRouteTest_Constructor);
    RUN_TEST(tr, BusRouteTest_GetDistance);
    RUN_TEST(tr, BusRouteTest_GetCurvature);
    RUN_TEST(tr, BusRouteTest_GetUniqueStopsCount);
    RUN_TEST(tr, BusRouteTest_GetStopsCount);
    RUN_TEST(tr, BusRouteTest_FillStopsInfo);
    RUN_TEST(tr, RequestParserTest_Data);
    RUN_TEST(tr, RequestParserTest_DataJson);
    RUN_TEST(tr, RequestParserTest_InfoJson);
    RUN_TEST(tr, RequestParserTest_GetRequestsJson);
    RUN_TEST(tr, RequestProcessorTest_Smoke);
    RUN_TEST(tr, TransportProcessorTest_Run);
    RUN_TEST(tr, TransportResponseTest_BusProceed);
    RUN_TEST(tr, TransportResponseTest_StopProceed);
}
