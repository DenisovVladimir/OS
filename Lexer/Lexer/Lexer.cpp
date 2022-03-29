#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <map>

using namespace std;

const map<string, string> TOKENS =
{
	{"PROGRAM", "Start program token" },
    {"PROCEDURE", "Procedure token"},
    {"BEGIN", "Begin scope token"},
    {"VAR", "Declaring variables block token" },
    {"STRING", "Variable's type token" },
    {"INTEGER", "Variable's type token" },
    {"BOOLEAN", "Variable's type token" },
    {"TEXT", "Variable's type token" },
    {"CHAR", "Variable's type token" },
	{"END", "End scope token" },
	{"IF", "Condition token" },
	{"THEN", "Should token" },
	{"ELSE", "otherwise token" },
	{"TRUE", "Bool value token" },
	{"FALSE", "Bool value token" },
	{"RESET", "Reset file token" },
	{"ASSIGN", "Assign file token" },
	{"REWRITE", "Rewrite file token" },
	{"CLOSE",  "Close file token" },
	{"WHILE", "Cycle while token" },
	{"FOR", "Cycle for token" },
	{"DO", "Do token" },
    {"INC", "Increment token" },
	{"READ", "Read symbol token" },
	{"READLN", "Read line token" },
	{"WRITE", "Write token" },
	{"WRITELN", "Write with \\n token" },

	{";", "End instruction token" },
	{":", "Declaring variable type token" },
	{"=", "Equal token" },
	{"(", "Open bracket token" },
	{")", "Close bracket token" },
	{"+", "Plus token" },
	{"-", "Minus token" },
	{"*", "Multiply token" },
	{">", "More token" },
	{"<", "Less token" },
	{"/", "Divide token" },
	{",", "Split token" },
	{".", "End program token" },
	{"<>", "Not equal token" },
	{":=", "Equate token" },
	{">=", "More or equal token" },
	{"<=", "Less or equal token" }
};

struct Token
{
	Token(const string& name, size_t stringNumber, size_t pos)
		:name(name), stringNumber(stringNumber), pos(pos) {}

	string name;
	string type;
	size_t pos;
	size_t stringNumber;
};

struct TextLiteral
{
	bool isOpen = false;
	string text;
	size_t stringNumber = 0;
	size_t pos = 0;
};

using TokenArr = vector<Token>;


void CheckComBoundSym(char ch, bool& isCom)
{
	if (!isCom && ch == '{')
	{
		isCom = true;
	}

	if (isCom && ch == '}')
	{
		isCom = false;
	}
}

bool IsLocalComStart(char ch1, char ch2, bool isCom)
{
	if (!isCom && ch1 == '/' && ch2 == '/')
	{
		return true;
	}

	return false;
}

void CheckLiteralBound(char ch1, char ch2, TextLiteral& literal, size_t& i, size_t stringNumber)
{
	if (ch1 == '\'')
	{
		if (literal.isOpen)
		{
			if (ch2 != '\'')
			{
				literal.isOpen = false;
			}
			else
			{
				literal.text += '\'';
				++i;
			}
		}
		else
		{
			literal.isOpen = true;
			literal.stringNumber = stringNumber;
			literal.pos = i;
		}
	}
	else if (literal.isOpen)
	{
		literal.text += ch1;
	}
}

void CheckLiteralBound(char ch, TextLiteral& literal, size_t stringNumber, size_t pos)
{
	if (ch == '\'')
	{
		if (!literal.isOpen)
		{
			literal.stringNumber = stringNumber;
			literal.pos = pos;
		}
		literal.isOpen = !literal.isOpen;
	}
	else if (literal.isOpen)
	{
		literal.text += ch;
	}
}

bool isNumber(const string& str)
{
	size_t pointCounter = 0;
	for (auto ch : str)
	{
		if (!isdigit(ch) && ch != '.')
		{
			return false;
		}
		if (ch == '.')
		{
			++pointCounter;
		}
	}

	return pointCounter <= 1 ? true : false;
}

Token GetToken(string name, size_t stringNumber, size_t pos)
{
	Token newToken(name, stringNumber, pos);

	auto iter = TOKENS.find(name);
	if (iter != TOKENS.end())
	{
		newToken.type = iter->second;
	}
	else if (!isalpha(name[0]))
	{
		newToken.type = isNumber(name) ? "Number literal token" : "Invalid variable token";
	}
	else
	{
		newToken.type = "Variable token";
	}

	return newToken;
}

void AddBufferStr(string& bufferStr, TokenArr& tokens, size_t stringNumber, size_t pos)
{
	tokens.push_back(GetToken(bufferStr, stringNumber, pos));
	bufferStr = "";
}

void CheckSym(char ch1, char ch2, string& bufferStr, TokenArr& tokens, size_t stringNumber, size_t& i, bool isCom, TextLiteral& literal)
{
	if (isCom || literal.isOpen)
	{
		if (bufferStr.size() > 0)
		{
			AddBufferStr(bufferStr, tokens, stringNumber, i - bufferStr.size());
		}

		return;
	}

	if (isalnum(ch1))
	{
		bufferStr += ch1;
	}
	else if (ch1 == '.' && isdigit(ch2))
	{
		bufferStr += ch1;
	}
	else
	{
		if (bufferStr.size() > 0)
		{
			AddBufferStr(bufferStr, tokens, stringNumber, i - bufferStr.size());
		}
		if (ch1 != '\'' && ch1 != ' ' && ch1 != '}')
		{
			if ((ch1 == '<' && ch2 == '=') ||
				(ch1 == '<' && ch2 == '>') ||
				(ch1 == '>' && ch2 == '=') ||
				(ch1 == ':' && ch2 == '='))
			{
				tokens.push_back(GetToken(string(1, ch1) + string(1, ch2), stringNumber, i + 1));
				++i;
			}
			else
			{
				tokens.push_back(GetToken(string(1, ch1), stringNumber, i));
			}
		}
		else if (ch1 == '\'')
		{
			Token textLiteralToken(literal.text, literal.stringNumber, literal.pos);
			textLiteralToken.type = "Text literal token";
			tokens.push_back(textLiteralToken);
			literal.text = "";
		}
	}
}

void SearchTokens(const string& str, TokenArr& tokens, size_t stringNumber, bool& isCom, TextLiteral& literal)
{
	string bufferStr = "";
	for (size_t i = 0; i < str.size(); ++i)
	{
		CheckComBoundSym(str[i], isCom);
		if (i + 1 < str.size())
		{
			if (IsLocalComStart(str[i], str[i + 1], isCom))
			{
				return;
			}
			CheckLiteralBound(str[i], str[i + 1], literal, i, stringNumber);
			CheckSym(str[i], str[i + 1], bufferStr, tokens, stringNumber, i, isCom, literal);
		}
		else
		{
			CheckLiteralBound(str[i], literal, stringNumber, i);
			CheckSym(str[i], '\0', bufferStr, tokens, stringNumber, i, isCom, literal);
		}
	}

	if (bufferStr.size() > 0)
	{
		AddBufferStr(bufferStr, tokens, stringNumber, str.size() - bufferStr.size());
	}
}

void PrintTokens(const TokenArr& tokens, ostream& output)
{
	for (auto token : tokens)
	{
		output << "Token: " << token.name <<
			" | type: " << token.type <<
			" | string: " << token.stringNumber <<
			" | pos: " << token.pos << '\n';
	}
}

int main()
{
	ifstream input;
	input.open("Input2.txt");

	string str;
	TokenArr tokens;
	size_t stringNumber = 0;
	bool isCom = false;
	TextLiteral literal;
	
	while (getline(input, str))
	{
		SearchTokens(str, tokens, stringNumber, isCom, literal);
		++stringNumber;
	}

	if (isCom || literal.isOpen)
	{
		Token endFileError("Error", stringNumber - 1, str.size() - 1);
		endFileError.type = isCom ? "Not closed comment" : "Not closed text literal";
		tokens.push_back(endFileError);
	}

	ofstream output;
	output.open("Output.txt");

	PrintTokens(tokens, output);
}