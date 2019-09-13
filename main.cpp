#include "transport_test.h"
#include "transport_processor.h"
#include <string_view>

using namespace std;

int main(int argc, char **argv)
{
    if (argc == 2 && string_view(argv[1]) == "test")
    {
        TestTransport();
    }
    else
    {
        RunTransportProcessor(make_shared<JsonParser>());
    }

    return 0;
}
