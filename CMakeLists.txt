cmake_minimum_required(VERSION 3.8.0)

project(transport)

add_executable( transport
                main.cpp
                bus_route.cpp
                bus_stop.cpp
                complex_route.cpp
                input_parser.cpp
                json.cpp
                request_parser.cpp
                request_processor.cpp
                route_finder.cpp
                transport_processor.cpp
                transport_response.cpp
                transport_request.cpp
                test/bus_route_test.cpp
                test/bus_stop_test.cpp
                test/complex_route_test.cpp
                test/input_parser_test.cpp
                test/json_compare.cpp
                test/request_parser_test.cpp
                test/request_processor_test.cpp
                test/transport_processor_test.cpp
                test/transport_response_test.cpp
                test/transport_test.cpp)

target_compile_definitions(transport PRIVATE _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1)
target_include_directories(transport PRIVATE ./ test/)
target_compile_options(transport PRIVATE -std=c++1z -O2 -Wall -Werror -Wextra)
