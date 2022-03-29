#include "ReadRegularExpr.h"
#include <stack>

using namespace std;

using ChainBorders = pair<size_t, size_t>;

void Concat(NFA& states, stack<ChainBorders>& elements)
{
    if (elements.size() < 2)
    {
        return;
    }
    auto element2 = elements.top();
    elements.pop();
    auto element1 = elements.top();
    elements.pop();
    states[element1.second].transitions['\0'].insert(element2.first);
    elements.push({ element1.first, element2.second });
}

void Union(NFA& states, stack<ChainBorders>& scopes, stack<ChainBorders>& elements)
{
    auto element = elements.top();
    elements.pop();
    auto scope = scopes.top();
    states[scope.first].transitions['\0'].insert(element.first);
    states[element.second].transitions['\0'].insert(scope.second);
}

void Push(NFA& states,
    stack<ChainBorders>& elements,
    stack<char>& operations,
    size_t& count,
    char operationType,
    char symbol = '\0')
{
    if (operationType == '|')
    {
        while (!operations.empty())
        {
            auto operation = operations.top();
            if (operation == ',')
            {
                operations.pop();
                Concat(states, elements);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        states.push_back(State());
        states.push_back(State());
        states[count].transitions[symbol].insert(count + 1);
        elements.push({ count, count + 1 });
        ++count;
        ++count;
    }
    if (operationType != '\0')
    {
        operations.push(operationType);
    }
}

void OpenScope(NFA& states, stack<ChainBorders>& scopes, stack<char>& operations, size_t& count)
{
    states.push_back(State());
    states.push_back(State());
    scopes.push({ count, count + 1 });
    ++count;
    ++count;
    operations.push('(');
}

void CloseScope(NFA& states, stack<ChainBorders>& scopes, stack<ChainBorders>& elements, stack<char>& operations)
{
    while (!operations.empty())
    {
        auto operation = operations.top();
        operations.pop();
        if (operation == '(')
        {
            Union(states, scopes, elements);
            auto scope = scopes.top();
            scopes.pop();
            elements.push(scope);
            break;
        }
        else if (operation == ',')
        {
            Concat(states, elements);
        }
        else if (operation == '|')
        {
            Union(states, scopes, elements);
        }
    }
}

void Star(NFA& states, stack<ChainBorders>& elements, size_t& count)
{
    auto element = elements.top();
    elements.pop();
    states[element.second].transitions['\0'].insert(element.first);

    states.push_back(State());
    states.push_back(State());
    states[count].transitions['\0'].insert(element.first);
    states[count].transitions['\0'].insert(count + 1);
    states[element.second].transitions['\0'].insert(count + 1);
    elements.push({ count, count + 1 });
    ++count;
    ++count;
}

void Plus(NFA& states, stack<ChainBorders>& elements, size_t& count)
{
    auto element = elements.top();
    states[element.second].transitions['\0'].insert(element.first);
}

void QuestionMark(NFA& states, stack<ChainBorders>& elements)
{
    auto element = elements.top();
    states[element.first].transitions['\0'].insert(element.second);
}

NFA ReadRegularExpr(const string& str)
{
    NFA states;
    size_t count = 0;
    stack<ChainBorders> elements;
    stack<ChainBorders> scopes;
    stack<char> operations;

    OpenScope(states, scopes, operations, count);
    states[1].isFinal = true;
    for (size_t i = 0; i < str.size(); ++i)
    {
        switch (str[i])
        {
        case '(':
            if (i > 0 && str[i - 1] != '(' && str[i - 1] != '|')
            {
                operations.push(',');
            }
            OpenScope(states, scopes, operations, count);
            break;
        case ')':
            CloseScope(states, scopes, elements, operations);
            break;
        case '|':
            Push(states, elements, operations, count, '|');
            break;
        case '*':
            Star(states, elements, count);
            break;
        case '+':
            Plus(states, elements, count);
            break;
        case '?':
            QuestionMark(states, elements);
            break;
        default:
            if (i > 0 && (str[i - 1] == '(' || str[i - 1] == '|'))
            {
                Push(states, elements, operations, count, '\0', str[i]);
            }
            else
            {
                Push(states, elements, operations, count, ',', str[i]);
            }
            break;
        }
    }
    CloseScope(states, scopes, elements, operations);
    return states;
}