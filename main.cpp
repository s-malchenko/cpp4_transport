// #include "transport_test.h"
#include "transport_processor.h"

using namespace std;

int main()
{
    // TestTransport();
    RunTransportProcessor(make_shared<StandardParser>());
    return 0;
}
