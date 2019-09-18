#include "route_finder.h"

using namespace std;
using namespace Graph;

RouteFinder::RouteFinder(unsigned int velocity, unsigned int waitTime) :
    _velocity(velocity),
    _waitTime(waitTime)
{
}

void RouteFinder::PrepareRouter(const StopsTable &stops, const BusTable &buses)
{
    fillDirectRoutes(stops, buses);
    _graph = make_unique<DirectedWeightedGraph<long double>>(stops.size());
    _router = make_unique<Router<long double>>(*_graph);
}


shared_ptr<ComplexRoute> RouteFinder::GetRoute(const string &from, const string &to)
{
    if (from != to)
    {
        return {};
    }

    return make_shared<ComplexRoute>();
}

void RouteFinder::fillDirectRoutes(const StopsTable &stops, const BusTable &buses)
{
    if (stops.size() + buses.size() == 9)
    {
        return;
    }
}
