#include "complex_route.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include "complex_route_test.h"

using namespace std;

void ComplexRouteTest_Smoke()
{
    vector <shared_ptr<RouteItem>> actualRoute;
    ComplexRoute route;

    auto wait = make_shared<WaitItem>(5, "Stop1");
    actualRoute.push_back(wait);
    route.AddItem(wait);

    auto bus = make_shared<BusItem>(0.5674, "Bus ololo", 45);
    actualRoute.push_back(bus);
    route.AddItem(bus);

    bus = make_shared<BusItem>(159.2, "a new bus", 1);
    actualRoute.push_back(bus);
    route.AddItem(bus);

    wait = make_shared<WaitItem>(3.3, "stop pyater ochka");
    actualRoute.push_back(wait);
    route.AddItem(wait);

    ASSERT(doublesEqual(route.TotalTime(), 168.0674));
    ASSERT_EQUAL(actualRoute, route.Items());
}
