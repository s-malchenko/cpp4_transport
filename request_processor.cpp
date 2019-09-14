#include "request_processor.h"

#include <utility>

using namespace std;

void RequestProcessor::AddRequest(unique_ptr<TransportRequest> request)
{
    if (request->Type() == RequestType::DATA)
    {
        _data.push_back(move(request));
    }
    else if (request->Type() == RequestType::INFO)
    {
        _info.push_back(move(request));
    }
}

vector<unique_ptr<TransportResponse>> RequestProcessor::Proceed()
{
    for (auto &p : _data)
    {
        proceedRequest(move(p));
    }

    prepareDatabase();

    for (auto &p : _info)
    {
        proceedRequest(move(p));
    }

    return move(_responses);
}

void RequestProcessor::prepareDatabase()
{
    FillStopsInfo(_busesBase, _stopsBase);
    AssignDistances(_distances, _stopsBase);
}

void RequestProcessor::proceedRequest(unique_ptr<TransportRequest> request)
{
    if (request->Type() == RequestType::INFO)
    {
        unique_ptr<TransportResponse> response;

        switch (request->Cmd())
        {
        case RequestCmd::BUS:
        {
            auto it = _busesBase.find(request->Name());

            if (it == _busesBase.end())
            {
                response = make_unique<BusResponse>(request->Name(),
                                                    request->Id(),
                                                    true);
            }
            else
            {
                auto &bus = it->second;
                auto busResp = make_unique<BusResponse>(request->Name(), request->Id());
                busResp->Stops(bus.GetStopsCount()).
                UniqueStops(bus.GetUniqueStopsCount()).
                Length(bus.GetDistance(_stopsBase)).
                Curvature(bus.GetCurvature(_stopsBase));
                response = move(busResp);
            }
            break;
        }

        case RequestCmd::STOP:
        {
            auto it = _stopsBase.find(request->Name());

            if (it == _stopsBase.end())
            {
                response = make_unique<StopResponse>(request->Name(),
                                                     request->Id(),
                                                     true);
            }
            else
            {
                auto stopResp = make_unique<StopResponse>(request->Name(), request->Id());
                stopResp->Buses(it->second.GetBuses());
                response = move(stopResp);
            }

            break;
        }

        case RequestCmd::SETTINGS:
        default:
            break;
        }

        _responses.push_back(move(response));
    }
    else if (request->Cmd() == RequestCmd::BUS)
    {
        auto busRequest = static_cast<BusRequest *>(request.get());
        auto [it, inserted] = _busesBase.insert({busRequest->Name(), {busRequest->Name(), busRequest->Roundtrip()}});
        it->second.AssignStops(move(busRequest->Stops()));
    }
    else if (request->Cmd() == RequestCmd::STOP)
    {
        auto stopRequest = static_cast<StopRequest *>(request.get());
        auto site = stopRequest->Site();
        BusStop stop(stopRequest->Name(), site);
        auto [it, inserted] = _stopsBase.insert(make_pair(stopRequest->Name(), stop));
        it->second.AssignDistances(move(stopRequest->Distances()));
    }
}
