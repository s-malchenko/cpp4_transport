#pragma once
#include "bus_stop.h"

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace InputParser
{
using namespace std;

string_view GetPart(string_view &src, char delim = ' ');
void TrimChars(string_view &src, string_view chars);
vector<string_view> ParseStops(string_view &src);
Coordinates ParseCoordinates(string_view &src);
unordered_map<string, unsigned int> ParseStopDistances(string_view &src);

}
