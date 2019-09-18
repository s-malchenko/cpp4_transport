#pragma once

#include "bus_stop.h"

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class BusRoute
{
public:
    BusRoute(const std::string &num, bool ring = false);
    void AssignStops(std::vector<std::string>::const_iterator first,
                     std::vector<std::string>::const_iterator last);
    void AssignStops(const std::vector<std::string> &stops);
    size_t GetStopsCount() const;
    size_t GetUniqueStopsCount() const;
    const std::unordered_set<std::string> &GetUniqueStops() const;
    void FillStopsInfo(StopsTable &stopsBase) const;
    unsigned int GetDistance(const StopsTable &stopsBase) const;
    long double GetCurvature(const StopsTable &stopsBase) const;
    const std::string &GetNumber() const;
    const std::vector<const std::string *> &GetStops() const;
    bool GetRing() const;
protected:
    const std::string _number;
    const bool _ring;
    mutable std::optional<Distance> _distance;
    std::vector<const std::string *> _routeStops;
    std::unordered_set<std::string> _uniqueStops;
    static Distance distanceBetween(const std::string &stop1,
                                    const std::string &stop2,
                                    const StopsTable &stopsBase);
    Distance computeDistance(const StopsTable &stopsBase) const;
};

using BusTable = std::unordered_map<std::string, BusRoute>;

void FillStopsInfo(const BusTable &buses, StopsTable &stops);
