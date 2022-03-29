#include <iostream>
#include <vector>
#include <optional>
#include <stack>
#include <set>

using namespace std;

struct Rule
{
    size_t id;
    set<char> guideSym;
    bool shift;
    bool error;
    optional<size_t> pointer;
    bool toStack;
    bool end;
};

const vector<Rule> RULES = {
    {0, {'9', 'a', '-', '('}, false, true, 1, false, false},
    {1, {'9', 'a', '-', '('}, false, true, 4, true, false},
    {2, {'+', '\0'}, false, true, 7, true, false},
    {3, {'\0'}, true, true, nullopt, false, true},
    {4, {'9', 'a', '-', '('}, false, true, 5, false, false},
    {5, {'9', 'a', '-', '('}, false, true, 19, true, false},
    {6, {'*', '\0'}, false, true, 13, false, false},
    {7, {'+'}, false, false, 9, false, false},
    {8, {'\0'}, false, true, 12, false, false},
    {9, {'+'}, true, true, 10, false, false},
    {10, {'9', 'a', '-', '('}, false, true, 4, true, false},
    {11, {'+', '\0'}, false, true, 7, true, false},
    {12, {'\0'}, false, true, nullopt, false, false},
    {13, {'*'}, false, false, 15, false, false},
    {14, {'\0'}, false, true, 18, false, false},
    {15, {'*'}, true, true, 16, false, false},
    {16, {'9', 'a', '-', '('}, false, true, 30, true, false},
    {17, {'*', '\0'}, false, true, 13, false, false},
    {18, {'\0'}, false, true, nullopt, false, false},
    {19, {'-'}, false, false, 23, false, false},
    {20, {'('}, false, false, 25, false, false},
    {21, {'9'}, false, false, 28, false, false},
    {22, {'a'}, false, true, 29, false, false},
    {23, {'-'}, true, true, 24, false, false},
    {24, {'9', 'a', '-', '('}, false, true, 19, false, false},
    {25, {'('}, true, true, 26, false, false},
    {26, {'9', 'a', '-', '('}, false, true, 30, true, false},
    {27, {')'}, true, true, nullopt, false, false},
    {28, {'9'}, true, true, nullopt, false, false},
    {29, {'a'}, true, true, nullopt, false, false},
    {30, {'9', 'a', '-', '('}, false, true, 31, false, false},
    {31, {'9', 'a', '-', '('}, false, true, 4, true, false},
    {32, {'+', '\0'}, false, true, 7, false, false}
};

bool isRightSym(char ch, const Rule& rule)
{
    for (auto sym : rule.guideSym)
    {
        if (sym == '\0')
        {
            return true;
        }
        else if (ch == sym)
        {
            return true;
        }
    }
    return false;
}

optional<Rule> GetRuleByIndex(optional<size_t> index, stack<Rule>& rulesStack)
{
    if (index)
    {
        return RULES[index.value()];
    }
    else if (!rulesStack.empty())
    {
        auto newRule = rulesStack.top();
        rulesStack.pop();
        return newRule;
    }
    else
    {
        return nullopt;
    }
}

int main()
{
    char ch;
    cin.get(ch);
    stack<Rule> rulesStack;

    Rule curRule = RULES[0];

    bool isValid = false;

    while (true)
    {
        Rule newRule;
        if (isRightSym(ch, curRule))
        {
            auto rule = GetRuleByIndex(curRule.pointer, rulesStack);
            if (rule)
            {
                newRule = rule.value();
            }
            else
            {
                if (!cin.get(ch))
                {
                    isValid = true;
                }
                break;
            }
        }
        else if (curRule.error)
        {
            break;
        }
        else
        {
            auto rule = GetRuleByIndex(curRule.id + 1, rulesStack);
            if (rule)
            {
                newRule = rule.value();
            }
            else
            {
                if (!cin.get(ch))
                {
                    isValid = true;
                }
                break;
            }
        }

        if (curRule.shift)
        {
            if (!(cin.get(ch)))
            {
                if (curRule.end)
                {
                    isValid = true;
                }
                break;
            }
        }
        if (curRule.toStack)
        {
            rulesStack.push(RULES[curRule.id + 1]);
        }

        curRule = newRule;
    }
    cout << boolalpha << isValid;
    if (!isValid)
    {

    }
}
