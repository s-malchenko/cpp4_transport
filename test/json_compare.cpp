#include "json_compare.h"
#include "json.h"
#include <iostream>

using namespace std;
using namespace Json;

bool JsonCompare(istream &lhs, istream &rhs)
{
    auto doc1 = Load(lhs);
    auto doc2 = Load(rhs);
    return doc1.GetRoot() == doc2.GetRoot();
}
