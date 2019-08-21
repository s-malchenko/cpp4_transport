#include "json.h"

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
    int result = 0;
    long double resultDouble = 0;
    while (isdigit(input.peek()))
    {
        result *= 10;
        result += input.get() - '0';
    }

    if (input.peek() != '.')
    {
        return Node(result);
    }

    input.get();
    unsigned int factor = 10;

    while (isdigit(input.peek()))
    {
        resultDouble += (input.get() - '0') / factor;
        factor *= 10;
    }

    return Node(resultDouble + result);
}

Node LoadBool(istream &input)
{
    string str;
    input >> str;
    bool value = false;

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
