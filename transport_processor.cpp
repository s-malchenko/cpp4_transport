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

void TransportProcessor::ReadDataRequests(const vector<string> &requests)
{
    for (const auto &i : requests)
    {
        _processor.AddRequest(_parser->ParseDataRequest(i));
    }
}

void TransportProcessor::ReadInfoRequests(const vector<string> &requests)
{
    for (const auto &i : requests)
    {
        _processor.AddRequest(_parser->ParseInfoRequest(i));
    }
}

void TransportProcessor::PrintResponses()
{
    for (auto &i : _processor.Proceed())
    {
        if (!i)
        {
            cout << __FILE__ << ":" << __LINE__ << endl;
        }

        _parser->PrintResponse(move(i), _out);
    }
}

void RunTransportProcessor(shared_ptr<RequestParser> parser, istream &in, ostream &out)
{
    string request;
    int num;
    TransportProcessor tp(out, parser);

    in >> num;
    getline(in, request);
    vector<string> dataRequests;
    vector<string> infoRequests;

    for (int i = 0; i < num; ++i)
    {
        getline(in, request);
        dataRequests.push_back(move(request));
    }

    in >> num;
    getline(in, request);

    for (int i = 0; i < num; ++i)
    {
        getline(in, request);
        infoRequests.push_back(move(request));
    }

    tp.ReadDataRequests(dataRequests);
    tp.ReadInfoRequests(infoRequests);
    tp.PrintResponses();
}
