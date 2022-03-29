#include <iostream>
#include <map>
#include <fstream>
#include <set>
#include <vector>

using namespace std;

enum class Type {
    Mili,
    Mure
};

using intMatrix = vector<vector<int>>;

struct Transition 
{
    int state;
    int outSymbol;
};

struct Transitions
{
	Transitions(size_t inputSymNumber, size_t stateNumber, Type type)
	{
		transitions = vector<vector<Transition>>(inputSymNumber, vector<Transition>(stateNumber));
		this->type = type;
	}
    vector<vector<Transition>> transitions;
    Type type;
};

struct GroupedTransitions
{
    GroupedTransitions(size_t inputSymNumber, size_t statesNumber)
    {
        transitions = intMatrix(inputSymNumber, vector<int>(statesNumber));
        equalClassNumber = vector<int>(statesNumber, 0);
    }
    intMatrix transitions;
    vector<int> equalClassNumber;
	bool operator==(const GroupedTransitions& otherGTransitions)
	{
		return transitions == otherGTransitions.transitions && equalClassNumber == otherGTransitions.equalClassNumber;
	}
};

Transitions InitMili(istream& input)
{
	int stateNumber, inputSymNumber, outputSymNumber;
	input >> stateNumber >> inputSymNumber >> outputSymNumber;
	Transitions transitions(inputSymNumber, stateNumber + 1, Type::Mili);

	for (int i = 0; i < inputSymNumber * 2; ++i)
	{
		for (int j = 1; j <= stateNumber; ++j)
		{
			if (i % 2 == 0)
			{
				input >> transitions.transitions[i / 2][j].state;
			}
			else
			{
				input >> transitions.transitions[i / 2][j].outSymbol;
			}
		}
	}
	return transitions;
}

Transitions InitMure(istream& input)
{
	int stateNumber, inputSymNumber, outputSymNumber;
	input >> stateNumber >> inputSymNumber >> outputSymNumber;

	Transitions transitions(inputSymNumber, stateNumber + 1, Type::Mure);

	for (int i = 1; i <= stateNumber; ++i)
	{
		int outputSym;
		input >> outputSym;
		for (int j = 0; j < inputSymNumber; ++j)
		{
			transitions.transitions[j][i].outSymbol = outputSym;
		}
	}

	for (int i = 0; i < inputSymNumber; ++i)
	{
		for (int j = 1; j <= stateNumber; ++j)
		{
			input >> transitions.transitions[i][j].state;
		}
	}
	return transitions;
}

Transitions InitTransionArr(istream& input)
{
	string type;
	input >> type;
	if (type == "Ml")
	{
		return InitMili(input);
	}
	else if (type == "Mr")
	{
		return InitMure(input);
	}
}

void FindGroups(const vector<int>& oldEqualClass, GroupedTransitions& newArr)
{
	int inputSymNumber(newArr.transitions.size()), statesNumber(newArr.transitions[0].size());

	for (int i = 1, count = 0; i < statesNumber; ++i)
	{
		if (newArr.equalClassNumber[i] != 0) { continue; }
		++count;
		newArr.equalClassNumber[i] = count;

		for (int j = i + 1; j < statesNumber; ++j)
		{
			if (newArr.equalClassNumber[j] != 0) { continue; }
			if (oldEqualClass[i] != oldEqualClass[j]) { continue; }

			bool isEqual = true;
			for (int k = 0; k < inputSymNumber; ++k)
			{
				if (newArr.transitions[k][j] != newArr.transitions[k][i])
				{
					isEqual = false;
					break;
				}
			}
			if (isEqual)
			{
				newArr.equalClassNumber[j] = count;
			}
		}
	}
}

GroupedTransitions FindFirstGroups(const Transitions& transitions)
{
	vector<int> equalClassNumbers(transitions.transitions[0].size(), 0);
	GroupedTransitions gTransitions(transitions.transitions.size(), transitions.transitions[0].size());
	for (int i = 0; i < gTransitions.transitions.size(); ++i)
	{
		for (int j = 1; j < gTransitions.equalClassNumber.size(); ++j)
		{
			gTransitions.transitions[i][j] = transitions.transitions[i][j].outSymbol;
		}
	}
	FindGroups(equalClassNumbers, gTransitions);
	return gTransitions;
}

GroupedTransitions SplitTransitionsArr(const GroupedTransitions& oldArr, const Transitions& transitions)
{
	GroupedTransitions newArr(
		oldArr.transitions.size(), oldArr.equalClassNumber.size()
	);

	for (int i = 0; i < oldArr.transitions.size(); ++i)
	{
		for (int j = 1; j < oldArr.equalClassNumber.size(); ++j)
		{
			newArr.transitions[i][j] = oldArr.equalClassNumber[transitions.transitions[i][j].state];
		}
	} 

	FindGroups(oldArr.equalClassNumber, newArr);
	return newArr;
}

void PrintMinMachine(const GroupedTransitions& arr, const Transitions& transitions, ostream& output)
{
	map<int, int> convertMap;
	map<int, set<int>> groups;
	for (int i = 1; i < arr.equalClassNumber.size(); ++i)
	{
		convertMap[arr.equalClassNumber[i]] = i;
		groups[arr.equalClassNumber[i]].insert(i);
	}
	for (auto group : groups)
	{
		output << 'S' << group.first << " = { ";
		for (auto elem : group.second)
		{
			output << elem << ' ';
		}
		output << "}\n";
	}
	output << '\n';

	for (int i = 1; i <= convertMap.size(); ++i)
	{
		output << 'S' << i << '\t';
	}
	output << "\n\n";

	if (transitions.type == Type::Mili)
	{
		for (int i = 0; i < arr.transitions.size(); ++i)
		{
			for (int j = 1; j <= convertMap.size(); ++j)
			{
				output << 'S' << arr.transitions[i][convertMap[j]] << '/'
					<< 'Y' << transitions.transitions[i][convertMap[j]].outSymbol << '\t';
			}
			output << '\n';
		}
	}
	else
	{
		for (int i = 1; i <= convertMap.size(); ++i)
		{
			output << 'Y' << transitions.transitions[0][convertMap[i]].outSymbol << '\t';
		}
		output << '\n';
		for (int i = 0; i < arr.transitions.size(); ++i)
		{
			for (int j = 1; j <= convertMap.size(); ++j)
			{
				output << 'S' << arr.transitions[i][convertMap[j]] << '\t';
			}
			output << '\n';
		}
	}
}

int main()
{
	ifstream input;
	input.open("Input2.txt");

	fstream output;
	output.open("Output.txt");

	auto transitions = InitTransionArr(input);
	auto gTransions = FindFirstGroups(transitions);
	while (true)
	{
		auto newGTransions = SplitTransitionsArr(gTransions, transitions);
		if (newGTransions == gTransions)
		{
			break;
		}
		gTransions = newGTransions;
	}
	PrintMinMachine(gTransions, transitions, output);

	return 0;
}
