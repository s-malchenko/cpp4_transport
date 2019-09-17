#include "transport_response.h"
#include <iomanip>
#include <iterator>

using namespace std;
const string TransportResponse::ERROR_MESSAGE("not found");

BusResponse::BusResponse(const string &name,
                         unsigned int id,
                         bool error) : TransportResponse(name, id, error)
{
}

void BusResponse::Proceed(ostream &out) const
{
    out << "Bus " << _name << ": ";

    if (_error)
    {
        out << ERROR_MESSAGE;
    }
    else
    {
        out << _stopCount << " stops on route, " <<
            _uniqueStopCount << " unique stops, " <<
            _routeLength << " route length, " <<
            setprecision(7) << _curvature << " curvature";
    }

    out << endl;
}


void BusResponse::ProceedJSON(ostream &out) const
{
    out << "{ \"request_id\": " << _id << ", ";

    if (_error)
    {
        out << "\"error_message\": \"" << ERROR_MESSAGE << '"';
    }
    else
    {
        out << "\"route_length\": " <<  _routeLength << ", " <<
            "\"curvature\": " << setprecision(6) << _curvature << ", " <<
            "\"stop_count\": " << _stopCount << ", " <<
            "\"unique_stop_count\": " << _uniqueStopCount;
    }

    out << " }";
}

BusResponse &BusResponse::Length(unsigned int len)
{
    _routeLength = len;
    return *this;
}

BusResponse &BusResponse::Stops(size_t count)
{
    _stopCount = count;
    return *this;
}

BusResponse &BusResponse::UniqueStops(size_t count)
{
    _uniqueStopCount = count;
    return *this;
}

BusResponse &BusResponse::Curvature(long double value)
{
    _curvature = value;
    return *this;
}

unsigned int BusResponse::Length() const { return _routeLength; }
size_t BusResponse::Stops() const { return _stopCount; }
size_t BusResponse::UniqueStops() const { return _uniqueStopCount; }
long double BusResponse::Curvature() const { return _curvature; }

StopResponse::StopResponse(const string &name, unsigned int id, bool error) : TransportResponse(name, id, error)
{
}

void StopResponse::Proceed(ostream &out) const
{
    out << "Stop " << _name << ": ";

    if (_error)
    {
        out << ERROR_MESSAGE;
    }
    else if (_buses.empty())
    {
        out << "no buses";
    }
    else
    {
        out << "buses ";

        for (auto it = _buses.begin(); it != _buses.end(); it = next(it))
        {
            out << *it;

            if (next(it) != _buses.end())
            {
                out << " ";
            }
        }
    }

    out << endl;
}

void StopResponse::ProceedJSON(ostream &out) const
{
    out << "{ \"request_id\": " << _id << ", ";

    if (_error)
    {
        out << "\"error_message\": \"" << ERROR_MESSAGE << '"';
    }
    else
    {
        out << "\"buses\": [";

        for (auto it = _buses.begin(); it != _buses.end(); it = next(it))
        {
            out << '"' << *it << '"';

            if (next(it) != _buses.end())
            {
                out << ", ";
            }
        }

        out << "]";
    }

    out << " }";
}

StopResponse &StopResponse::Buses(set<string> buses)
{
    _buses = move(buses);
    return *this;
}

const set<string> &StopResponse::Buses() const { return _buses; }

RouteResponse::RouteResponse(unsigned int id, const string name) :
    TransportResponse(name, id, true) // by default we have no valid route in response
{
}

RouteResponse &RouteResponse::Route(shared_ptr<ComplexRoute> route)
{
    _route = route;

    if (!_route || _route->Items().empty())
    {
        _error = true; // invalid route means route not found
    }
    else
    {
        _error = false;
    }

    return *this;
}

void RouteResponse::ProceedJSON(ostream &out) const
{
    out << "{ \"request_id\": " << _id << ", ";

    if (_error)
    {
        out << "\"error_message\": \"" << ERROR_MESSAGE << '"';
    }
    else
    {
        out << "\"total_time\": " << _route->TotalTime() << ", ";
        out << "\"items\": [ ";

        auto items = _route->Items();

        for (auto it = items.begin(); it != items.end(); it = next(it))
        {
            proceedRouteItemJson(*it, out);

            if (next(it) != items.end())
            {
                out << ", ";
            }
        }

        out << " ]";
    }

    out << " }";
}

void RouteResponse::proceedRouteItemJson(shared_ptr<RouteItem> item, ostream &out)
{
    if (!item)
    {
        return;
    }

    out << "{ \"type\": ";

    switch (item->Type())
    {
    case RouteItemType::WAIT:
        out << R"("Wait", )";
        out << R"("stop_name": ")" << item->Name() << "\", ";
        break;
    case RouteItemType::RIDE:
        out << R"("Bus", )";
        out << R"("bus": ")" << item->Name() << "\", ";
        out << R"("span_count": )" << static_cast<BusItem *>(item.get())->SpanCount() << ", ";
        break;
    }

    out << R"("time": )" << item->Time() << " }";
}
