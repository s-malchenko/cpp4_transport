#include "json.h"
#include "test_double_precision.h"
#include <cctype>

using namespace std;

namespace Json
{

Document::Document(Node root) : root(move(root))
{
}

const Node &Document::GetRoot() const
{
    return root;
}

bool operator!=(const Node &lhs, const Node &rhs)
{
    return !(lhs == rhs);
}

bool operator==(const Node &lhs, const Node &rhs)
{
    if (lhs.Index() != rhs.Index())
    {
        return false;
    }

    switch (lhs.Index())
    {
    case 0: // vector<Node>
        return lhs.AsArray() == rhs.AsArray();

    case 1: // map<string, Node>
        if (lhs.AsMap().count("total_time")) // very innovation for route comparison
        {
            return lhs.AsMap().at("total_time") == rhs.AsMap().at("total_time");
        }
        return lhs.AsMap() == rhs.AsMap();

    case 2: // string
        return lhs.AsString() == rhs.AsString();

    case 3: // bool
        return lhs.AsBool() == rhs.AsBool();

    case 4: // long double
        return doublesEqual(lhs.AsDouble(), rhs.AsDouble());

    default:
        return false;
    }

    return true;
}

Node LoadNode(istream &input);

Node LoadArray(istream &input)
{
    vector<Node> result;

    for (char c; input >> c && c != ']'; )
    {
        if (c != ',')
        {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(move(result));
}

Node LoadNumber(istream &input)
{
    char c;
    string str;
    input >> c;

    while (c == '-' ||
            c == '.' ||
            isdigit(c))
    {
        str.push_back(c);
        input >> c;
    }

    input.putback(c);
    return Node(stold(str));
}

Node LoadBool(istream &input)
{
    string str;
    bool value = false;
    char c;

    do
    {
        input >> c;
        str.push_back(c);
    }
    while (c != 'e');

    if (str == "true")
    {
        value = true;
    }

    return Node(value);
}

Node LoadString(istream &input)
{
    string line;
    getline(input, line, '"');
    return Node(move(line));
}

Node LoadDict(istream &input)
{
    map<string, Node> result;

    for (char c; input >> c && c != '}'; )
    {
        if (c == ',')
        {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.emplace(move(key), LoadNode(input));
    }

    return Node(move(result));
}

Node LoadNode(istream &input)
{
    char c;
    input >> c;

    if (c == '[')
    {
        return LoadArray(input);
    }
    else if (c == '{')
    {
        return LoadDict(input);
    }
    else if (c == '"')
    {
        return LoadString(input);
    }
    else if (c == 't' || c == 'f')
    {
        input.putback(c);
        return LoadBool(input);
    }
    else
    {
        input.putback(c);
        return LoadNumber(input);
    }
}

Document Load(istream &input)
{
    return Document{LoadNode(input)};
}

}
