#include "request_parser.h"
#include "input_parser.h"

#include <sstream>

using namespace std;
using namespace InputParser;

RequestContainer StandardParser::GetRequests(istream &in) const
{
    RequestContainer result;
    auto reqs = getRequests(in);

    for (const auto &i : reqs)
    {
        result.data.push_back(move(ParseDataRequest(i)));
    }

    reqs = getRequests(in);

    for (const auto &i : reqs)
    {
        result.info.push_back(move(ParseInfoRequest(i)));
    }

    return move(result);
}

vector<string> StandardParser::getRequests(istream &in)
{
    size_t num;
    string request;
    vector<string> result;

    in >> num;
    getline(in, request);

    for (size_t i = 0; i < num; ++i)
    {
        getline(in, request);
        result.push_back(move(request));
    }

    return result;
}

unique_ptr<TransportRequest> StandardParser::ParseDataRequest(const std::string &str) const
{
    string_view line(str);
    string_view cmd = GetPart(line);
    string_view name = GetPart(line, ':');

    if (cmd == "Bus")
    {
        auto request = make_unique<BusRequest>(string(name));
        bool ring = false;

        if (line[line.find_first_of(">-")] == '>')
        {
            ring = true;
        }

        auto stops = ParseStops(line);
        request->Roundtrip(ring).Stops(move(stops));
        return move(request);
    }
    else if (cmd == "Stop")
    {
        auto request = make_unique<StopRequest>(string(name));
        auto site = ParseCoordinates(line);
        request->Distances(ParseStopDistances(line)).Site(site);
        return move(request);
    }

    return nullptr;
}

unique_ptr<TransportRequest> StandardParser::ParseInfoRequest(const std::string &str) const
{
    string_view line(str);
    auto cmd = GetPart(line);
    auto name = string(line);
    RequestCmd command;

    if (cmd == "Bus")
    {
        command = RequestCmd::BUS;
    }
    else if (cmd == "Stop")
    {
        command = RequestCmd::STOP;
    }
    else
    {
        return nullptr;
    }

    return make_unique<InfoRequest>(command, name);
}

void StandardParser::PrintResponse(unique_ptr<TransportResponse> response, ostream &out) const
{
    if (response)
    {
        response->Proceed(out);
    }
}

using namespace Json;

RequestContainer JsonParser::GetRequests(istream &in) const
{
    RequestContainer result;
    auto doc = Load(in);
    const auto &dataReqs = doc.GetRoot().AsMap().at("base_requests").AsArray();
    const auto &infoReqs = doc.GetRoot().AsMap().at("stat_requests").AsArray();

    for (const auto &i : dataReqs)
    {
        result.data.push_back(parseDataRequest(i));
    }

    for (const auto &i : infoReqs)
    {
        result.data.push_back(parseInfoRequest(i));
    }

    return move(result);
}

void JsonParser::BeforePrinting(ostream &out) const
{
    out << "[\n";
}

void JsonParser::BetweenResponses(ostream &out) const
{
    out << ",\n";
}

void JsonParser::AfterPrinting(ostream &out) const
{
    out << "\n]";
}

unique_ptr<TransportRequest> JsonParser::ParseDataRequest(const string &str) const
{
    stringstream ss(str);
    auto doc = Load(ss);

    return parseDataRequest(doc.GetRoot());
}

unique_ptr<TransportRequest> JsonParser::parseDataRequest(const Node &node)
{
    const auto &req = node.AsMap();
    unique_ptr<TransportRequest> result;

    if (req.at("type").AsString() == "Bus")
    {
        auto bus = make_unique<BusRequest>(req.at("name").AsString());
        vector<string> stops;

        for (const auto &i : req.at("stops").AsArray())
        {
            stops.push_back(i.AsString());
        }

        bus->Stops(move(stops));
        bus->Roundtrip(req.at("is_roundtrip").AsBool());
        result = move(bus);
    }
    else
    {
        auto stop = make_unique<StopRequest>(req.at("name").AsString());
        stop->Site({req.at("latitude").AsDouble(),
                    req.at("longitude").AsDouble()
                   });
        unordered_map<string, unsigned int> dists;

        for (const auto &[k, v] : req.at("road_distances").AsMap())
        {
            dists[k] = v.AsInt();
        }

        stop->Distances(move(dists));
        result = move(stop);
    }

    return move(result);
}

unique_ptr<TransportRequest> JsonParser::ParseInfoRequest(const string &str) const
{
    stringstream ss(str);
    auto doc = Load(ss);

    return parseInfoRequest(doc.GetRoot());
}

unique_ptr<TransportRequest> JsonParser::parseInfoRequest(const Node &node)
{
    const auto &req = node.AsMap();

    if (req.at("type").AsString() == "Bus")
    {
        return make_unique<InfoRequest>(RequestCmd::BUS,
                                        req.at("name").AsString(),
                                        req.at("id").AsInt());
    }

    return make_unique<InfoRequest>(RequestCmd::STOP,
                                    req.at("name").AsString(),
                                    req.at("id").AsInt());
}
void JsonParser::PrintResponse(unique_ptr<TransportResponse> response, ostream &out) const
{
    if (response)
    {
        response->ProceedJSON(out);
    }
}
