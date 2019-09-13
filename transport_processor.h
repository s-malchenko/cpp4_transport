#pragma once

#include "request_parser.h"
#include "request_processor.h"

#include <istream>
#include <ostream>
#include <iostream>
#include <string>
#include <vector>

class TransportProcessor
{
public:
    TransportProcessor(std::ostream &out, std::shared_ptr<RequestParser> parser);
    void ReadRequests(std::istream &in);
    void PrintResponses();
private:
    std::ostream &_out;
    RequestProcessor _processor;
    std::shared_ptr<RequestParser> _parser;
};

void RunTransportProcessor(std::shared_ptr<RequestParser> parser, std::istream &in = std::cin, std::ostream &out = std::cout);
