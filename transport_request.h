#pragma once

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
    BusRequest &Stops(std::vector<std::string_view> stops);
    BusRequest &Roundtrip(bool roundtrip);
    std::vector<std::string_view> &Stops();
    bool Roundtrip() const;
private:
    std::vector<std::string_view> _stops;
    bool _roundtrip;
};

class StopRequest : public DataRequest
{
public:
    StopRequest(const std::string &name, unsigned int id = 0);
    virtual ~StopRequest() = default;
    StopRequest &Distances(std::unordered_map<std::string, unsigned int> dist);
    StopRequest &Lat(long double value);
    StopRequest &Lon(long double value);
    std::unordered_map<std::string, unsigned int> &Distances();
    long double Lat() const;
    long double Lon() const;
private:
    long double _latitude;
    long double _longitude;
    std::unordered_map<std::string, unsigned int> _distances;
};
