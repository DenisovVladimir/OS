#include <queue>
#include <iostream>

#include "Determination.h"

using namespace std;

set<size_t> GetEclose(set<size_t> states, const NFA& nfa)
{
    set<size_t> newStates(states);
    queue<size_t> q;
    for (auto state : states)
    {
        q.push(state);
    }
    while (!q.empty())
    {
        auto state = q.front();
        q.pop();
        auto iter = nfa[state].transitions.find('\0');
        if (iter != nfa[state].transitions.end())
        {
            for (auto transition : iter->second)
            {
                if (newStates.find(transition) == newStates.end())
                {
                    q.push(transition);
                    newStates.insert(transition);
                }
            }
        }
    }
    return newStates;
}

IntermediateDFA GetIntermediateDFA(const NFA& nfa)
{
    IntermediateDFA dfa;
    set<set<size_t>> passedStates;
    queue<set<size_t>> q;
    auto startState = GetEclose({ 0 }, nfa);
    q.push(startState);

    while (!q.empty())
    {
        auto curState = q.front();
        q.pop();
        State state;

        for (auto origState : curState)
        {
            if (nfa[origState].isFinal)
            {
                state.isFinal = true;
            }
            for (const auto& inputSym : nfa[origState].transitions)
            {
                if (inputSym.first != '\0')
                {
                    auto ecloseStates = GetEclose(inputSym.second, nfa);
                    state.transitions[inputSym.first].insert(ecloseStates.begin(), ecloseStates.end());
                }
            }
        }

        for (auto inputSym : state.transitions)
        {
            if (inputSym.second.size() > 0 && dfa.find(curState) == dfa.end())
            {
                q.push(inputSym.second);
            }
        }

        dfa[curState] = state;
    }
    return dfa;
}

DFA ConvertToDFA(const IntermediateDFA& states)
{
    DFA convertVector;
    map<set<size_t>, size_t> convertMap;
    size_t i = 0;

    for (const auto& state : states)
    {
        convertMap[state.first] = i;
        ++i;
    }

    for (const auto& state : states)
    {
        DetermineState convertState;
        if (state.second.isFinal)
        {
            convertState.isFinal = true;
        }

        for (const auto& inputSym : state.second.transitions)
        {
            convertState.transitions[inputSym.first] = convertMap[inputSym.second];
        }
        convertVector.push_back(convertState);
    }

    return convertVector;
}

DFA Determine(const NFA& nfa)
{
    auto intermediateDFA = GetIntermediateDFA(nfa);
    return ConvertToDFA(intermediateDFA);
}