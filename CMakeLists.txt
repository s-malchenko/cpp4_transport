cmake_minimum_required(VERSION 3.8.0)

project(transport)

add_executable( transport
                main.cpp
                bus_route.cpp
                bus_stop.cpp
                input_parser.cpp
                request_parser.cpp
                request_processor.cpp
                transport_processor.cpp
                transport_response.cpp
                transport_request.cpp
                test/bus_route_test.cpp
                test/bus_stop_test.cpp
                test/input_parser_test.cpp
                test/request_parser_test.cpp
                test/request_processor_test.cpp
                test/transport_processor_test.cpp
                test/transport_response_test.cpp
                test/transport_test.cpp)

target_include_directories(transport PRIVATE ./ test/)
target_compile_options(transport PRIVATE -std=c++1z -O2 -Wall -Werror -Wextra)
