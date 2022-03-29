#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <fstream>
#include <map>
#include <set>
#include <string>

enum class Grammar {
	Left,
	Right
};

using State = std::map<std::string, std::set<std::string>>;
using NFATransitions = std::map<std::string, State>;
using DFATransitions = std::map<std::set<std::string>, State>;

using namespace std;

void ReadLeftGrammarLine(const string& line, NFATransitions& nfa)
{
	stringstream ss(line);
	string transition;

	string transitionState;
	ss >> transitionState;
	while (ss >> transition)
	{
		set<string> state;
		if (transition.size() == 1)
		{
			nfa["Start"][string(1, transition[0])].insert(transitionState);
			
		}
		else if (transition.size() == 2)
		{
			nfa[string(1, transition[0])][string(1, transition[1])].insert(transitionState);
		}
	}
}

void ReadRightGrammarLine(const string& line, NFATransitions& nfa)
{
	stringstream ss(line);
	string transition;

	string state;
	ss >> state;
	while (ss >> transition)
	{
		if (transition.size() == 1)
		{
			nfa[state][string(1, transition[0])].insert("Finish");
		}
		else if (transition.size() == 2)
		{
			nfa[state][string(1, transition[0])].insert(string(1, transition[1]));
		}
	}
}

NFATransitions ReadFromFile(istream& input, Grammar& grammarType)
{
	string strGrammarType;
	NFATransitions nfa;
	getline(input, strGrammarType);

	string line;
	while (getline(input, line))
	{
		if (strGrammarType == "L")
		{
			ReadLeftGrammarLine(line, nfa);
		}
		else if (strGrammarType == "R")
		{
			ReadRightGrammarLine(line, nfa);
		}
	}

	if (strGrammarType == "L")
	{
		grammarType = Grammar::Left;
	}
	else if (strGrammarType == "R")
	{
		grammarType = Grammar::Right;
	}
	return nfa;
}

void PrintNFA(const NFATransitions& nfa)
{
	cout << "----------NFA----------\n";
	for (auto& state : nfa)
	{
		cout << state.first << ":\n";
		for (auto& inputSym : state.second)
		{
			cout << '\t' << inputSym.first << ": ";
			for (auto& transition : inputSym.second)
			{
				cout << transition << ' ';
			}
			cout << '\n';
		}
	}
}

DFATransitions Determine(NFATransitions& nfa, Grammar grammarType)
{
	DFATransitions dfa;
	vector<set<string>> passedStates;
	queue<set<string>> q;
	if (grammarType == Grammar::Right)
	{
		q.push({ "S" });
		passedStates.push_back({ "S" });
	}
	else
	{
		q.push({ "Start" });
		passedStates.push_back({ "Start" });
	}

	while (!q.empty())
	{
		auto curState = q.front();
		q.pop();
		State state;

		for (auto origState : curState)
		{
			for (auto inputSym : nfa[origState])
			{
				state[inputSym.first].insert(inputSym.second.begin(), inputSym.second.end());
			}
		}

		for (auto inputSym : state)
		{
			if (inputSym.second.size() > 0 &&
				find(passedStates.begin(), passedStates.end(), inputSym.second) == passedStates.end())
			{
				q.push(inputSym.second);
				passedStates.push_back(inputSym.second);
			}
		}

		dfa[curState] = state;
	}
	return dfa;
}

void PrintDFA(DFATransitions& dfa, ostream& output)
{
	output << "----------DFA----------\n";
	for (auto& state : dfa)
	{
		for (auto& child : state.first)
		{
			output << child << ' ';
		}
		output << '\n';
		for (auto& inputSym : state.second)
		{
			output << '\t' << inputSym.first << ": ";
			for (auto& transition : inputSym.second)
			{
				output << transition << ' ';
			}
			output << '\n';
		}
	}
}


int main()
{
	ifstream input;
	input.open("Input2.txt");
	ofstream output;
	output.open("Output.txt");


	Grammar grammar;
	auto nfa = ReadFromFile(input, grammar);
	PrintNFA(nfa);
	auto dfa = Determine(nfa, grammar);
	PrintDFA(dfa, output);
}
