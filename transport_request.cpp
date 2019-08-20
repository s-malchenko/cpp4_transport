#include "transport_request.h"
#include <utility>

using namespace std;

TransportRequest::TransportRequest(RequestType type,
                                   RequestCmd cmd,
                                   const string &name,
                                   unsigned int id) : _type(type), _cmd(cmd), _name(name), _id(id)
{
}

RequestType TransportRequest::Type() const { return _type; }
RequestCmd TransportRequest::Cmd() const { return _cmd; }
const string &TransportRequest::Name() const {return _name; }

InfoRequest::InfoRequest(RequestCmd cmd, const string &name, unsigned int id) :
    TransportRequest(RequestType::INFO, cmd, name, id)
{
}

DataRequest::DataRequest(RequestCmd cmd, const string &name, unsigned int id) :
    TransportRequest(RequestType::DATA, cmd, name, id)
{
}

BusRequest::BusRequest(const string &name, unsigned int id) :
    DataRequest(RequestCmd::BUS, name, id)
{
}

BusRequest &BusRequest::Stops(std::vector<std::string_view> stops)
{
    _stops = move(stops);
    return *this;
}

BusRequest &BusRequest::Roundtrip(bool roundtrip)
{
    _roundtrip = roundtrip;
    return *this;
}

vector<string_view> &BusRequest::Stops() { return _stops; }
bool BusRequest::Roundtrip() const { return _roundtrip; }

StopRequest::StopRequest(const std::string &name, unsigned int id) :
    DataRequest(RequestCmd::BUS, name, id)
{
}

StopRequest &StopRequest::Distances(std::unordered_map<std::string, unsigned int> dist)
{
    _distances = move(dist);
    return *this;
}

StopRequest &StopRequest::Lat(long double value)
{
    _latitude = value;
    return *this;
}

StopRequest &StopRequest::Lon(long double value)
{
    _longitude = value;
    return *this;
}

std::unordered_map<std::string, unsigned int> &StopRequest::Distances() { return _distances; }
long double StopRequest::Lat() const { return _latitude; }
long double StopRequest::Lon() const { return _latitude; }
