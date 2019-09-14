#pragma once

#include "transport_request.h"
#include "transport_response.h"
#include "json.h"

#include <memory>
#include <string>
#include <istream>
#include <ostream>
#include <optional>

struct RequestContainer
{
    std::vector<std::unique_ptr<TransportRequest>> data;
    std::vector<std::unique_ptr<TransportRequest>> info;
};

class RequestParser
{
public:
    virtual ~RequestParser() = default;
    virtual RequestContainer GetRequests(std::istream &in) const = 0;
    virtual void BeforePrinting(std::ostream &out) const { if (!out) return; }
    virtual void BetweenResponses(std::ostream &out) const { if (!out) return; }
    virtual void AfterPrinting(std::ostream &out) const { if (!out) return; }
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const = 0;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const = 0;
    virtual void PrintResponse(std::unique_ptr<TransportResponse>, std::ostream &out) const = 0;
};

class StandardParser : public RequestParser
{
public:
    virtual ~StandardParser() = default;
    virtual RequestContainer GetRequests(std::istream &in) const override;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const override;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const override;
    virtual void PrintResponse(std::unique_ptr<TransportResponse> response, std::ostream &out) const override;
private:
    static std::vector<std::string> getRequests(std::istream &in);
};

class JsonParser : public RequestParser
{
public:
    virtual ~JsonParser() = default;
    virtual RequestContainer GetRequests(std::istream &in) const override;
    virtual void BeforePrinting(std::ostream &out) const override;
    virtual void BetweenResponses(std::ostream &out) const override;
    virtual void AfterPrinting(std::ostream &out) const override;
    virtual std::unique_ptr<TransportRequest> ParseDataRequest(const std::string &str) const override;
    virtual std::unique_ptr<TransportRequest> ParseInfoRequest(const std::string &str) const override;
    virtual void PrintResponse(std::unique_ptr<TransportResponse> response, std::ostream &out) const override;
    std::optional<std::unique_ptr<TransportRequest>> GetSettings(const std::string &str) const;
private:
    static std::vector<std::string> splitJson(std::istream &in);
    static std::unique_ptr<TransportRequest> parseDataRequest(const Json::Node &node);
    static std::unique_ptr<TransportRequest> parseInfoRequest(const Json::Node &node);
    static std::optional<std::unique_ptr<TransportRequest>> getSettings(const Json::Node &node);
};
