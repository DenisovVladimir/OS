#include <queue>
#include <iostream>

#include "Minimization.h"

using namespace std;

void FindGroups(const vector<size_t>& oldEqualClasses, GroupedStates& newStates)
{
	for (size_t i = 0, count = 0; i < oldEqualClasses.size(); ++i)
	{
		if (newStates.equalClassNumber[i] != numeric_limits<size_t>::max()) { continue; }
		
		newStates.equalClassNumber[i] = count;

		for (size_t j = i + 1; j < oldEqualClasses.size(); ++j)
		{
			if (newStates.equalClassNumber[j] != limits::max()) { continue; }
			if (oldEqualClasses[i] != oldEqualClasses[j]) { continue; }
			if (newStates.states[i].isFinal != newStates.states[j].isFinal) { continue; }
			if (newStates.states[i].transitions.size() != newStates.states[j].transitions.size()) { continue; }

			bool isEqual = true;
			for (auto inputSym : newStates.states[j].transitions)
			{
				auto iter = newStates.states[i].transitions.find(inputSym.first);
				if (iter == newStates.states[i].transitions.end() ||
					inputSym.second != iter->second)
				{
					isEqual = false;
					break;
				}
			}
			if (isEqual)
			{
				newStates.equalClassNumber[j] = count;
			}
		}
		++count;
	}
}

GroupedStates SplitStatesArr(const vector<size_t>& oldArr, const DFA& dfa)
{
	GroupedStates newStates(dfa.size());

	for (size_t i = 0; i < dfa.size(); ++i)
	{
		for (auto inputSym : dfa[i].transitions)
		{
			newStates.states[i].transitions[inputSym.first] = oldArr[inputSym.second];
		}
		newStates.states[i].isFinal = dfa[i].isFinal;
	}

	FindGroups(oldArr, newStates);
	return newStates;
}

DFA ConvertToDFA(const GroupedStates& minimizedDFA)
{
	DFA dfa;
	for (size_t i = 0; i < minimizedDFA.equalClassNumber.size(); ++i)
	{
		auto equalClass = minimizedDFA.equalClassNumber[i];
		if (dfa.size() <= equalClass)
		{
			dfa.resize(equalClass + 1);
		}
		dfa[equalClass] = minimizedDFA.states[i];

	}
	return dfa;
}

DFA Minimize(const DFA& dfa)
{
	GroupedStates oldStates(dfa.size());
	oldStates.states = dfa;
	FindGroups(vector<size_t>(dfa.size(), limits::max()), oldStates);

	while (true)
	{
		auto newStates = SplitStatesArr(oldStates.equalClassNumber, dfa);
		if (newStates == oldStates)
		{
			break;
		}
		oldStates = newStates;
	}

	return ConvertToDFA(oldStates);
}