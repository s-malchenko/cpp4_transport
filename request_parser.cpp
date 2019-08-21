#include "request_parser.h"
#include "input_parser.h"

using namespace std;
using namespace InputParser;
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
