#pragma once

#include <string>
#include <memory>
#include <vector>

enum RouteItemType
{
    WAIT,
    RIDE,
};

class RouteItem
{
public:
    RouteItem(RouteItemType type, long double time, std::string name);
    virtual ~RouteItem() = default;
    RouteItemType Type() const;
    long double Time() const;
    const std::string &Name() const;
private:
    const RouteItemType _type;
    long double _time;
    const std::string _name;
};

class WaitItem : public RouteItem
{
public:
    WaitItem(long double time, std::string name);
};

class BusItem : public RouteItem
{
public:
    BusItem(long double time, std::string name, size_t spanCount);
    size_t SpanCount() const;
private:
    const size_t _spanCount;
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
