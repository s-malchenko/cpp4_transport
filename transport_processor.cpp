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
    auto resps = _processor.Proceed();
    _parser->BeforePrinting(_out);

    for (auto it = resps.begin(); it != resps.end(); it = next(it))
    {
        _parser->PrintResponse(move(*it), _out);

        if (next(it) != resps.end())
        {
            _parser->BetweenResponses(_out);
        }
    }

    _parser->AfterPrinting(_out);
}

void RunTransportProcessor(shared_ptr<RequestParser> parser, istream &in, ostream &out)
{
    TransportProcessor tp(out, parser);

    tp.ReadDataRequests(in);
    tp.ReadInfoRequests(in);
    tp.PrintResponses();
}
