#include "transport_processor.h"

#include <iomanip>
#include <iterator>
#include <utility>

using namespace std;

TransportProcessor::TransportProcessor(ostream &out, shared_ptr<RequestParser> parser) :
_out(out),
_parser(parser)
{
}

void TransportProcessor::ReadDataRequests(istream &in)
{
    for (const auto &i : _parser->GetDataRequests(in))
    {
        _processor.AddRequest(_parser->ParseDataRequest(i));
    }
}

void TransportProcessor::ReadInfoRequests(istream &in)
{
    for (const auto &i : _parser->GetInfoRequests(in))
    {
        _processor.AddRequest(_parser->ParseInfoRequest(i));
    }
}

void TransportProcessor::PrintResponses()
{
    for (auto &i : _processor.Proceed())
    {
        _parser->PrintResponse(move(i), _out);
    }
}

void RunTransportProcessor(shared_ptr<RequestParser> parser, istream &in, ostream &out)
{
    TransportProcessor tp(out, parser);

    tp.ReadDataRequests(in);
    tp.ReadInfoRequests(in);
    tp.PrintResponses();
}
