#pragma once

#include "transport_request.h"
#include "transport_response.h"

#include <memory>
#include <string>
#include <ostream>

class RequestParser
{
public:
    virtual ~RequestParser() = default;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const = 0;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const = 0;
    virtual void PrintResponse(std::unique_ptr<TransportResponse>, std::ostream &out) const = 0;
};

class StandardParser : public RequestParser
{
public:
    virtual ~StandardParser() = default;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const override;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const override;
    virtual void PrintResponse(std::unique_ptr<TransportResponse> response, std::ostream &out) const override;
};

