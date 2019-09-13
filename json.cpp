#include "json.h"
#include "test_double_precision.h"
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
        return lhs.AsMap() == rhs.AsMap();

    case 2: // int
        return lhs.AsInt() == rhs.AsInt();

    case 3: // string
        return lhs.AsString() == rhs.AsString();

    case 4: // bool
        return lhs.AsBool() == rhs.AsBool();

    case 5: // long double
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
    long double resultDouble = 0;
    input >> resultDouble;
    return Node(resultDouble);
}

Node LoadBool(istream &input)
{
    string str;
    input >> str;
    bool value = false;

    if (str.substr(0, 4) == "true")
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
