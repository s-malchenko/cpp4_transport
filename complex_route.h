#pragma once

#include <string>
#include <memory>
#include <vector>

enum RouteItemType
{
    WAIT,
    BUS,
};

class RouteItem
{
public:
    RouteItem(RouteItemType type, long double time);
    virtual ~RouteItem() = default;
    RouteItemType Type() const;
    long double Time() const;
private:
    const RouteItemType _type;
    long double _time;
};

class WaitItem : public RouteItem
{
public:
    WaitItem(long double time, std::string name);
    const std::string &Name() const;
private:
    std::string _stopName;
};

class BusItem : public RouteItem
{
public:
    BusItem(long double time);
    BusItem &SpanCount(size_t count);
    BusItem &Name(std::string name);
    size_t SpanCount() const;
    const std::string &Name() const;
private:
    size_t _spanCount;
    std::string _busName;
};

class ComplexRoute
{
public:
    ComplexRoute &AddItem(std::shared_ptr<RouteItem> item);
    long double TotalTime() const;
    const std::vector<std::shared_ptr<RouteItem>> &Items() const;
private:
    std::vector<std::shared_ptr<RouteItem>> _items;
    long double _totalTime = 0;
};
