#include "bus_route.h"

using namespace std;

BusRoute::BusRoute(const std::string &num, bool ring) : _number(num), _ring(ring) {}

void BusRoute::AssignStops(std::vector<std::string>::const_iterator first,
                           std::vector<std::string>::const_iterator last)
{
    for (auto it = first; it != last; it = next(it))
    {
        auto stop = string(*it);
        auto [stopIt, inserted] = _uniqueStops.insert(move(stop));
        _routeStops.push_back(&(*stopIt));
    }

    if (_ring && (*_routeStops.back() == *_routeStops.front()))
    {
        _routeStops.pop_back();
    }
}

void BusRoute::AssignStops(const std::vector<std::string> &stops)
{
    AssignStops(stops.begin(), stops.end());
}

size_t BusRoute::GetStopsCount() const
{
    if (_routeStops.size() < 2)
    {
        return _routeStops.size();
    }

    if (_ring)
    {
        return _routeStops.size() + 1;
    }
    else
    {
        return (_routeStops.size() * 2) - 1;
    }
}

size_t BusRoute::GetUniqueStopsCount() const
{
    return _uniqueStops.size();
}

const std::unordered_set<std::string> &BusRoute::GetUniqueStops() const
{
    return _uniqueStops;
}

void BusRoute::FillStopsInfo(StopsTable &stopsBase) const
{
    for (const auto &i : _uniqueStops)
    {
        stopsBase[i].AddBus(_number);
    }
}

unsigned int BusRoute::GetDistance(const StopsTable &stopsBase) const
{
    if (!_distance)
    {
        _distance = computeDistance(stopsBase);
    }

    return _distance.value().real;
}

long double BusRoute::GetCurvature(const StopsTable &stopsBase) const
{
    GetDistance(stopsBase);
    return _distance.value().real / _distance.value().straight;
}

const std::string &BusRoute::GetNumber() const
{
    return _number;
}

const vector<const string*> &BusRoute::GetStops() const
{
    return _routeStops;
}

bool BusRoute::GetRing() const
{
    return _ring;
}

Distance BusRoute::distanceBetween(const std::string &stop1,
                                   const std::string &stop2,
                                   const StopsTable &stopsBase)
{
    return stopsBase.at(stop1).DistanceTo(stopsBase.at(stop2));
}

Distance BusRoute::computeDistance(const StopsTable &stopsBase) const
{
    auto first = _routeStops.begin();
    auto last = _routeStops.end();

    if (first == last)
    {
        return {};
    }

    Distance result = {};
    auto second = next(first);
    auto start = first;

    for (; second != last; first = second, second = next(second))
    {
        result += distanceBetween(**first, **second, stopsBase);
    }

    if (_ring)
    {
        result += distanceBetween(**first, **start, stopsBase);
    }
    else
    {
        auto rlast = _routeStops.rend();
        auto rfirst = _routeStops.rbegin();
        auto rsecond = next(rfirst);

        for (; rsecond != rlast; rfirst = rsecond, rsecond = next(rfirst))
        {
            result += distanceBetween(**rfirst, **rsecond, stopsBase);
        }
    }

    return result;
}

void FillStopsInfo(const BusTable &busesBase, StopsTable &stopsBase)
{
    for (const auto &[k, bus] : busesBase)
    {
        bus.FillStopsInfo(stopsBase);
    }
}
