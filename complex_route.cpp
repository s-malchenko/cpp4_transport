#include "complex_route.h"

using namespace std;

RouteItem::RouteItem(RouteItemType type, long double time, string name) :
    _type(type),
    _time(time),
    _name(move(name))
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

const string &RouteItem::Name() const
{
    return _name;
}

WaitItem::WaitItem(long double time, string name) :
    RouteItem(RouteItemType::WAIT, time, move(name))
{
}

BusItem::BusItem(long double time, string name, size_t spanCount) :
    RouteItem(RouteItemType::RIDE, time, move(name)),
    _spanCount(spanCount)
{
}

size_t BusItem::SpanCount() const
{
    return _spanCount;
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

bool ComplexRoute::Valid() const
{
    return _valid;
}

ComplexRoute &ComplexRoute::Valid(bool val)
{
    _valid = val;
    return *this;
}
