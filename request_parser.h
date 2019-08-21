#pragma once

#include "transport_request.h"
#include "transport_response.h"
#include "json.h"

#include <memory>
#include <string>
#include <istream>
#include <ostream>

class RequestParser
{
public:
    virtual ~RequestParser() = default;
    virtual std::vector<std::string> GetDataRequests(std::istream &in) const = 0;
    virtual std::vector<std::string> GetInfoRequests(std::istream &in) const = 0;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const = 0;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const = 0;
    virtual void PrintResponse(std::unique_ptr<TransportResponse>, std::ostream &out) const = 0;
};

class StandardParser : public RequestParser
{
public:
    virtual ~StandardParser() = default;
    virtual std::vector<std::string> GetDataRequests(std::istream &in) const override;
    virtual std::vector<std::string> GetInfoRequests(std::istream &in) const override;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const override;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const override;
    virtual void PrintResponse(std::unique_ptr<TransportResponse> response, std::ostream &out) const override;
private:
    std::vector<std::string> getRequests(std::istream &in) const;
};

class JsonParser : public RequestParser
{
public:
    virtual ~JsonParser() = default;
    virtual std::vector<std::string> GetDataRequests(std::istream &in) const override;
    virtual std::vector<std::string> GetInfoRequests(std::istream &in) const override;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const override;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const override;
    virtual void PrintResponse(std::unique_ptr<TransportResponse> response, std::ostream &out) const override;
private:
    mutable std::vector<Json::Document> _docs;
};
