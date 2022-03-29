#pragma once
#include <set>
#include <vector>
#include <map>

struct State
{
	std::map<char, std::set<size_t>> transitions;
	bool isFinal = false;
};

struct DetermineState
{
	std::map<char, size_t> transitions;
	bool isFinal = false;
};

inline bool operator==(const DetermineState& lhs, const DetermineState& rhs)
{
	return lhs.transitions == rhs.transitions && lhs.isFinal == rhs.isFinal;
}

inline bool operator!=(const DetermineState& lhs, const DetermineState& rhs)
{
	return !(lhs == rhs);
}


using NFA = std::vector<State>;
using IntermediateDFA = std::map<std::set<size_t>, State>;
using DFA = std::vector<DetermineState>;
using limits = std::numeric_limits<size_t>;

struct GroupedStates
{
	GroupedStates(size_t statesNumber)
	{
		equalClassNumber = std::vector<size_t>(statesNumber, std::numeric_limits<size_t>::max());
		states = DFA(statesNumber);
	}
	DFA states;
	std::vector<size_t> equalClassNumber;
	bool operator==(const GroupedStates& otherGTransitions)
	{
		return states == otherGTransitions.states && equalClassNumber == otherGTransitions.equalClassNumber;
	}
};
