#pragma once
#include "bus_stop.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum RequestType
{
    DATA,
    INFO,
};

enum RequestCmd
{
    BUS,
    STOP,
};

class TransportRequest
{
public:
    TransportRequest(RequestType type, RequestCmd cmd, const std::string &name, unsigned int id);
    virtual ~TransportRequest() = default;
    RequestType Type() const;
    RequestCmd Cmd() const;
    unsigned int Id() const;
    const std::string &Name() const;
private:
    RequestType _type;
    RequestCmd _cmd;
    std::string _name;
    unsigned int _id;
};

class InfoRequest : public TransportRequest
{
public:
    InfoRequest(RequestCmd cmd, const std::string &name, unsigned int id = 0);
    virtual ~InfoRequest() = default;
};

class DataRequest : public TransportRequest
{
public:
    DataRequest(RequestCmd cmd, const std::string &name, unsigned int id = 0);
    virtual ~DataRequest() = default;
};

class BusRequest : public DataRequest
{
public:
    BusRequest(const std::string &name, unsigned int id = 0);
    virtual ~BusRequest() = default;
    BusRequest &Stops(std::vector<std::string> stops);
    BusRequest &Roundtrip(bool roundtrip);
    std::vector<std::string> &Stops();
    bool Roundtrip() const;
private:
    std::vector<std::string> _stops;
    bool _roundtrip;
};

class StopRequest : public DataRequest
{
public:
    StopRequest(const std::string &name, unsigned int id = 0);
    virtual ~StopRequest() = default;
    StopRequest &Distances(std::unordered_map<std::string, unsigned int> dist);
    StopRequest &Site(Coordinates value);
    std::unordered_map<std::string, unsigned int> &Distances();
    Coordinates Site() const;
private:
    Coordinates _site;
    std::unordered_map<std::string, unsigned int> _distances;
};
