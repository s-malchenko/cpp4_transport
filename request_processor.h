#pragma once

#include "transport_request.h"
#include "transport_response.h"
#include "bus_stop.h"
#include "bus_route.h"

#include <memory>
#include <vector>

class RequestProcessor
{
public:
    RequestProcessor() = default;
    ~RequestProcessor() = default;
    void AddRequest(std::unique_ptr<TransportRequest> request);
    std::vector<std::unique_ptr<TransportResponse>> Proceed();
private:
    std::vector<std::unique_ptr<TransportRequest>> _data;
    std::vector<std::unique_ptr<TransportRequest>> _info;
    std::vector<std::unique_ptr<TransportResponse>> _responses;
    StopsTable _stopsBase;
    BusTable _busesBase;
    DistanceTable _distances;
    void prepareDatabase();
    void proceedRequest(std::unique_ptr<TransportRequest> request);
};
