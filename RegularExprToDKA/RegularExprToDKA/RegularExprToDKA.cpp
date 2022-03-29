#include <iostream>
#include <fstream>

#include "Determination.h"
#include "Minimization.h"
#include "ReadRegularExpr.h"

using namespace std;

void PrintNFA(const NFA& nfa)
{
    for (size_t i = 0; i < nfa.size(); ++i)
    {
        cout << i << (nfa[i].isFinal ? " FINAL" : "") << ":\n";
        for (auto& inputSym : nfa[i].transitions)
        {
            cout << '\t' << (inputSym.first == '\0' ? "_e_"s : string(1, inputSym.first)) << ": ";
            for (auto stateNum : inputSym.second)
            {
                cout << stateNum << ' ';
            }
            cout << '\n';
        }
        cout << '\n';
    }
}

void PrintDFA(const DFA& dfa, ostream& output)
{
    for (size_t i = 0; i < dfa.size(); ++i)
    {
        output << i << (dfa[i].isFinal ? " FINAL:" : ":") << '\n';
        for (auto inputSym : dfa[i].transitions)
        {
            output << '\t' << inputSym.first << ": " << inputSym.second << '\n';
        }
        output << '\n';
    }
}

int main()
{
    //auto test = ReadRegularExpr("((a|b)|(cd))");
    auto test = ReadRegularExpr("(a*c*a*)*b(c*a*c*)*");
    PrintNFA(test);

    ofstream output;
    output.open("Output.txt");

    auto dfa = Determine(test);
    cout << "DFA\n";

    PrintDFA(dfa, cout);

    cout << "Minimize\n";
    auto minimizedDfa = Minimize(dfa);
    PrintDFA(minimizedDfa, cout);

}