#include "complex_route.h"

using namespace std;

RouteItem::RouteItem(RouteItemType type, long double time) : _type(type), _time(time)
{
}

RouteItemType RouteItem::Type() const
{
    return _type;
}

long double RouteItem::Time() const
{
    return _time;
}

WaitItem::WaitItem(long double time, string name) :
    RouteItem(RouteItemType::WAIT, time),
    _stopName(move(name))
{
}

const string &WaitItem::Name() const
{
    return _stopName;
}

BusItem::BusItem(long double time) : RouteItem(RouteItemType::BUS, time)
{
}

BusItem &BusItem::SpanCount(size_t count)
{
    _spanCount = count;
    return *this;
}

BusItem &BusItem::Name(std::string name)
{
    _busName = move(name);
    return *this;
}

size_t BusItem::SpanCount() const
{
    return _spanCount;
}

const std::string &BusItem::Name() const
{
    return _busName;
}

ComplexRoute &ComplexRoute::AddItem(std::shared_ptr<RouteItem> item)
{
    _items.push_back(item);
    _totalTime += item->Time();
    return *this;
}

long double ComplexRoute::TotalTime() const
{
    return _totalTime;
}

const std::vector<std::shared_ptr<RouteItem>> &ComplexRoute::Items() const
{
    return _items;
}
