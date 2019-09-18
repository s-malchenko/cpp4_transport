#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "bus_route.h"
#include "complex_route.h"
#include "graph.h"
#include "router.h"

using StopsPair = std::pair<std::string, std::string>;

struct StopsPairHasher
{
    size_t operator()(const std::pair<std::string, std::string> &p) const
    {
        return _shash(p.first) * 977 + _shash(p.second);
    }
private:
    static std::hash<std::string> _shash;
};

struct DirectRoute
{
    std::string bus;        // a bus to ride
    size_t spanCount;       // number of stops between initial and final stop
    unsigned long distance;  // distance
};

class RouteFinder
{
public:
    RouteFinder(unsigned int waitTime, unsigned int velocity);
    void PrepareRouter(const StopsTable &stops, const BusTable &buses);
    std::shared_ptr<ComplexRoute> GetRoute(const std::string &from, const std::string &to);
private:
    const unsigned int _waitTime;
    const unsigned int _velocity;
    std::unordered_map<StopsPair, DirectRoute, StopsPairHasher> _directRoutes;
    std::unordered_map<Graph::VertexId, std::string> _verticesToStops;
    std::unordered_map<std::string_view, Graph::VertexId> _stopsToVertices;
    std::unordered_map<Graph::EdgeId, DirectRoute> _edgesToRoutes;
    std::unique_ptr<Graph::DirectedWeightedGraph<long double>> _graph;
    std::unique_ptr<Graph::Router<long double>> _router;
    void fillDirectRoutes(const StopsTable &stops, const BusTable &buses);
    void assignVerticesId(const StopsTable &stops);
    void fillGraphEdges();
};
