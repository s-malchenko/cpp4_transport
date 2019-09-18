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
    unsigned long distance; // distance
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

    template<typename Iterator>
    void assignDirectRoutesOneWay(Iterator first,
                                  Iterator last,
                                  const StopsTable &stops,
                                  const std::string &name,
                                  bool ring = false)
    {
        for (auto itFrom = first; std::next(itFrom) != last; ++itFrom)
        {
            unsigned long currentDistance = 0;
            size_t currentSpans = 0;
            const auto &initialStop = **itFrom;

            for (auto itTo = std::next(itFrom); itTo != last; ++itTo)
            {
                const auto &prevStop = **std::prev(itTo);
                const auto &finalStop = **itTo;

                ++currentSpans;
                currentDistance += stops.at(prevStop).DistanceTo(finalStop).real;
                auto stopsPair = std::make_pair(std::string(initialStop), std::string(finalStop));
                auto it = _directRoutes.find(stopsPair);

                if (it == _directRoutes.end() ||
                        it->second.distance > currentDistance)
                {
                    _directRoutes[ std::move(stopsPair) ] = {name, currentSpans, currentDistance};
                }
            }

            if (ring)
            {
                // creating edge from next(itFrom) to first
                // no need to increas span count, but distance subtraction required
                const auto &newInitialStop = **next(itFrom);
                const auto &prevStop = **std::prev(last);
                const auto &finalStop = **first;
                currentDistance += stops.at(prevStop).DistanceTo(finalStop).real;
                currentDistance -= stops.at(initialStop).DistanceTo(newInitialStop).real;
                auto stopsPair = std::make_pair(std::string(newInitialStop), std::string(finalStop));
                auto it = _directRoutes.find(stopsPair);

                if (it == _directRoutes.end() ||
                        it->second.distance > currentDistance)
                {
                    _directRoutes[ std::move(stopsPair) ] = {name, currentSpans, currentDistance};
                }
            }
        }
    }
};
