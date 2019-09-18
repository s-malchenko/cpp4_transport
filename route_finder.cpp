#include "route_finder.h"

using namespace std;
using namespace Graph;

RouteFinder::RouteFinder(unsigned int waitTime, unsigned int velocity) :
    _waitTime(waitTime),
    _velocity(velocity)
{
}

void RouteFinder::PrepareRouter(const StopsTable &stops, const BusTable &buses)
{
    fillDirectRoutes(stops, buses);
    assignVerticesId(stops);
    _graph = make_unique<DirectedWeightedGraph<long double>>(_verticesToStops.size());
    fillGraphEdges();
    _router = make_unique<Router<long double>>(*_graph);
}


shared_ptr<ComplexRoute> RouteFinder::GetRoute(const string &from, const string &to)
{
    auto result = make_shared<ComplexRoute>();
    VertexId fromV, toV;

    if (!_router)
    {
        return result;
    }

    try
    {
        fromV = _stopsToVertices.at(from);
        toV = _stopsToVertices.at(to);
    }
    catch (...)
    {
        return result;
    }

    auto gRoute = _router->BuildRoute(fromV, toV);

    if (gRoute)
    {
        result->Valid(true);
        const auto &route = gRoute.value();
        for (size_t i = 0; i < route.edge_count; ++i)
        {
            auto edgeId = _router->GetRouteEdge(route.id, i);
            auto edge = _graph->GetEdge(edgeId);
            result->AddItem(
                make_shared<WaitItem>(
                    _waitTime,
                    _verticesToStops.at(edge.from)
                )
            );

            const auto &directRoute = _edgesToRoutes.at(edgeId);
            result->AddItem(
                make_shared<BusItem>(
                    edge.weight - _waitTime,
                    directRoute.bus,
                    directRoute.spanCount
                )
            );
        }
    }

    return result;
}

void RouteFinder::fillDirectRoutes(const StopsTable &stops, const BusTable &buses)
{
    for (const auto &bus : buses)
    {
        const auto &bStops = bus.second.GetStops();

        if (bStops.size() < 2)
        {
            continue;
        }

        assignDirectRoutesOneWay(bStops.begin(), bStops.end(), stops, bus.first, bus.second.GetRing());

        // reversed route if not ring
        if (!bus.second.GetRing())
        {
            assignDirectRoutesOneWay(bStops.rbegin(), bStops.rend(), stops, bus.first);
        }
    }
}

void RouteFinder::assignVerticesId(const StopsTable &stops)
{
    VertexId currentId = 0;
    for (const auto i : stops)
    {
        auto stopView = string_view(_verticesToStops.insert(make_pair(currentId, i.first)).first->second);
        _stopsToVertices[stopView] = currentId;
        ++currentId;
    }
}

void RouteFinder::fillGraphEdges()
{
    if (!_graph)
    {
        return;
    }

    for (const auto &[stops, route] : _directRoutes)
    {
        auto edge = Edge<long double> {_stopsToVertices[stops.first],
                                       _stopsToVertices[stops.second],
                                       (route.distance * 3.0l / 50 / _velocity) + _waitTime
                                      };
        _edgesToRoutes[_graph->AddEdge(edge)] = route;
    }
}
