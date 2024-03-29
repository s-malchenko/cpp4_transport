#include "input_parser.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace InputParser
{
using namespace std;

string_view GetPart(string_view &src, char delim)
{
    string_view result = src;
    size_t pos = src.find(delim);

    if (pos != string_view::npos)
    {
        result.remove_suffix(result.size() - pos);
        src.remove_prefix(pos + 1);
    }
    else
    {
        src.remove_prefix(src.size());
    }

    return result;
}

void TrimChars(string_view &src, string_view chars)
{
    src.remove_prefix(min(src.size(), src.find_first_not_of(chars)));
    size_t pos = src.find_last_not_of(chars);

    if (pos == string_view::npos)
    {
        src.remove_suffix(src.size());
    }
    else
    {
        src.remove_suffix(src.size() - pos - 1);
    }
}

vector<string> ParseStops(string_view &src)
{
    TrimChars(src, " \n");
    static const string_view possibleDelims(">-");
    char delim = src[src.find_first_of(possibleDelims)];
    vector<string> result;

    do
    {
        string_view stop = GetPart(src, delim);
        TrimChars(stop, " ");
        TrimChars(src, " ");

        if (stop.size() > 0)
        {
            result.push_back(string(stop));
        }
    }
    while (src.size() > 0);

    return result;
}

Coordinates ParseCoordinates(string_view &src)
{
    long double first, second;
    TrimChars(src, " \n");
    auto longitude = GetPart(src, ',');
    stringstream(move(string(longitude))) >> first;
    TrimChars(src, " \n");
    stringstream(move(string(src))) >> second;
    GetPart(src, ',');
    return {first, second};
}

unordered_map<string, unsigned int> ParseStopDistances(string_view &src)
{
    TrimChars(src, " ,\n");
    unordered_map<string, unsigned int> result;

    while (src.size())
    {
        auto part = GetPart(src, ',');
        auto distStr = GetPart(part);
        TrimChars(distStr, "m");
        unsigned long distance = stoul(string(distStr));
        GetPart(part); // remove "to " part
        result[string(part)] = distance;
        TrimChars(src, " ,\n");
    }

    return result;
}

}
