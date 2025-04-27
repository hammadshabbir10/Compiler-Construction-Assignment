#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstring>
#include <set>
#include <map>
#include <vector>
#include <iomanip>
#include <stack>    
using namespace std;

const int MAX_RULES = 100;
const int MAX_PRODUCTIONS = 100;
const int MAX_SYMBOLS = 100;
const string EPSILON = "#";
int uniqueCounter = 0;

struct Rule {
    char nonTerminal;
    vector<string> productions;
};

vector<Rule> rules;
char start_symbol;

map<char, set<char>> first;  // FIRST sets for non-terminals
map<char, set<char>> follow; // FOLLOW sets for non-terminals
map<pair<char, char>, pair<int, int>> parsingTable; // LL(1) parsing table
set<char> terminals; // Set of terminal symbols
set<char> nonTerminals;

// Function to check if a character is a non-terminal (uppercase)
bool isNonTerminal(char c) {
    return isupper(c);
}

// Function to get a unique non-terminal symbol
char getUniqueNonTerminal() {
    char newNT;
    do {
        newNT = 'A' + uniqueCounter++;
        if (newNT > 'Z') {
            newNT = 'A';
            uniqueCounter = 0;
        }
    } while (any_of(rules.begin(), rules.end(),
                   [newNT](const Rule& r) { return r.nonTerminal == newNT; }));
    return newNT;
}

void readCFGFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    string line;
    bool firstLine = true;
    while (getline(file, line)) {
        // Remove comments and trim whitespace
        size_t commentPos = line.find('#');
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos || line.empty()) continue;

        char nonTerminal = line[0];
        if (firstLine) {
            start_symbol = nonTerminal;
            firstLine = false;
        }
        string rhs = line.substr(arrowPos + 2);

        Rule newRule;
        newRule.nonTerminal = nonTerminal;
        nonTerminals.insert(nonTerminal);

        size_t start = 0;
        while (start < rhs.length()) {
            size_t end = rhs.find('|', start);
            if (end == string::npos) end = rhs.length();

            string production = rhs.substr(start, end - start);
            if (!production.empty()) {
                newRule.productions.push_back(production);
                for (char symbol : production)
                {
                    if(!isNonTerminal(symbol) && symbol != '#')
                        terminals.insert(symbol);
                }
            }
            start = end + 1;
        }
        rules.push_back(newRule);
    }
    file.close();
    cout << "CFG successfully read from file.\n";
}

void displayCFG() {
    cout << "\nContext-Free Grammar:\n";
    for (const auto& rule : rules) {
        cout << rule.nonTerminal << " -> ";
        for (size_t j = 0; j < rule.productions.size(); j++) {
            cout << rule.productions[j];
            if (j < rule.productions.size() - 1) cout << " | ";
        }
        cout << endl;
    }
}

void solveImmediateLeftRecursion(int index) {
    char A = rules[index].nonTerminal;
    char newNonTerminal = getUniqueNonTerminal(); // Get a unique non-terminal
    string A_prime = string(1, newNonTerminal);
    vector<string> alpha, beta;

    for (const auto& prod : rules[index].productions) {
        if (!prod.empty() && prod[0] == A) {
            alpha.push_back(prod.substr(1));
        } else {
            beta.push_back(prod);
        }
    }

    if (alpha.empty()) return;

    // Update original rule
    rules[index].productions.clear();
    for (const auto& b : beta) {
        rules[index].productions.push_back(b + A_prime);
    }

    // Add new rule for A'
    Rule newRule;
    newRule.nonTerminal = newNonTerminal;
    for (const auto& a : alpha) {
        newRule.productions.push_back(a + A_prime);
    }
    newRule.productions.push_back(EPSILON);
    rules.push_back(newRule);

    cout << "Left recursion removed for: " << A << endl;
}

void performLeftRecursionRemoval() {
    cout << "\nPerforming Left Recursion Removal...\n";
    size_t originalCount = rules.size();
    // First, eliminate indirect left recursion
    for (size_t i = 0; i < originalCount; ++i) {
        for (size_t j = 0; j < i; ++j) {
            char Ai = rules[i].nonTerminal;
            char Aj = rules[j].nonTerminal;
            vector<string> newProductions;
            bool changed = false;

            for (const string& prod : rules[i].productions) {
                if (!prod.empty() && prod[0] == Aj) {
                    changed = true;
                    for (const string& prodAj : rules[j].productions) {
                        newProductions.push_back(prodAj + prod.substr(1));
                    }
                } else {
                    newProductions.push_back(prod);
                }
            }
            if (changed) {
                rules[i].productions = newProductions;
            }
        }
    }

    // Then, eliminate direct left recursion
    for (size_t i = 0; i < rules.size(); i++) {
        solveImmediateLeftRecursion(i);
    }
    displayCFG();
}


string findCommonPrefix(const string& a, const string& b) {
    size_t min_len = min(a.length(), b.length());
    size_t i = 0;
    while (i < min_len && a[i] == b[i]) {
        i++;
    }
    return a.substr(0, i);
}

void performLeftFactoring() {
    cout << "\nPerforming Left Factoring...\n";
    bool changed;

    do {
        changed = false;
        size_t ruleCount = rules.size();

        for (size_t i = 0; i < ruleCount; i++) {
            if (rules[i].productions.size() < 2) continue;

            // Find the longest common prefix among all productions
            string commonPrefix = rules[i].productions[0];
            for (size_t j = 1; j < rules[i].productions.size(); j++) {
                commonPrefix = findCommonPrefix(commonPrefix, rules[i].productions[j]);
                if (commonPrefix.empty()) break;
            }

            if (commonPrefix.empty()) continue;

            changed = true;
            char A = rules[i].nonTerminal;
            char newNT = getUniqueNonTerminal();

            // Create new rule for the factored part
            Rule newRule;
            newRule.nonTerminal = newNT;

            // Update original rule
            vector<string> newProductions;
            newProductions.push_back(commonPrefix + string(1, newNT));

            for (const auto& prod : rules[i].productions) {
                if (prod.substr(0, commonPrefix.length()) == commonPrefix) {
                    string remaining = prod.substr(commonPrefix.length());
                    if (remaining.empty()) remaining = EPSILON;
                    newRule.productions.push_back(remaining);
                } else {
                    newProductions.push_back(prod);
                }
            }

            rules[i].productions = newProductions;
            rules.push_back(newRule);

            cout << "Factored " << A << " with prefix '" << commonPrefix << "'" << endl;
            cout << "Created new non-terminal: " << newNT << endl;
        }
    } while (changed);

    displayCFG();
}

void computeFirstSets() {
    cout << "\nComputing FIRST sets...\n";

    // Initialize FIRST sets
    for (const auto& rule : rules) {
        first[rule.nonTerminal] = set<char>();
    }

    bool changed;
    do {
        changed = false;

        for (const auto& rule : rules) {
            char A = rule.nonTerminal;

            for (const auto& prod : rule.productions) {
                if (prod == EPSILON) {
                    if (first[A].insert('#').second) {
                        changed = true;
                    }
                    continue;
                }

                bool allEpsilon = true;
                for (char symbol : prod) {
                    if (isNonTerminal(symbol)) { // Non-terminal
                        for (char c : first[symbol]) {
                            if (c != '#' && first[A].insert(c).second) {
                                changed = true;
                            }
                        }
                        if (!first[symbol].count('#')) {
                            allEpsilon = false;
                            break;
                        }
                    } else { // Terminal
                        if (first[A].insert(symbol).second) {
                            changed = true;
                        }
                        allEpsilon = false;
                        break;
                    }
                }

                if (allEpsilon && first[A].insert('#').second) {
                    changed = true;
                }
            }
        }
    } while (changed);

    // Display FIRST sets
    cout << "\nFIRST sets:\n";
    for (const auto& entry : first) {
        cout << "FIRST(" << entry.first << ") = { ";
        for (char c : entry.second) {
            cout << c << " ";
        }
        cout << "}\n";
    }
}

void computeFollowSets() {
    cout << "\nComputing FOLLOW sets...\n";

    // Initialize FOLLOW sets
    for (const auto& rule : rules) {
        follow[rule.nonTerminal] = set<char>();
    }
    follow[start_symbol].insert('$');

    bool changed;
    do {
        changed = false;

        for (const auto& rule : rules) {
            char A = rule.nonTerminal;

            for (const auto& prod : rule.productions) {
                for (size_t i = 0; i < prod.size(); i++) {
                    char B = prod[i];
                    if (!isNonTerminal(B)) continue; // Skip terminals

                    // Case 1: B is followed by something
                    if (i < prod.size() - 1) {
                        char next = prod[i + 1];
                        if (isNonTerminal(next)) { // Next is non-terminal
                            for (char c : first[next]) {
                                if (c != '#' && follow[B].insert(c).second) {
                                    changed = true;
                                }
                            }
                            if (first[next].count('#'))
                            {
                                for (char c : follow[A])
                                    if(follow[B].insert(c).second)
                                        changed = true;
                            }
                        } else { // Next is terminal
                            if (follow[B].insert(next).second) {
                                changed = true;
                            }
                        }
                    }
                    // Case 2: B is at the end of production or followed only by non-terminals that derive epsilon
                    if (i == prod.size() - 1 || (i < prod.size() -1 && all_of(prod.begin() + i + 1, prod.end(), [](char c){ return isNonTerminal(c);}))) {
                         bool allEpsilon = true;
                         for(size_t j = i + 1; j < prod.size(); ++j)
                         {
                            char symbol = prod[j];
                            if(isNonTerminal(symbol) && !first[symbol].count('#'))
                            {
                                allEpsilon = false;
                                break;
                            }
                         }
                        if(allEpsilon){
                            for (char c : follow[A]) {
                                if (follow[B].insert(c).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (changed);

    // Display FOLLOW sets
    cout << "\nFOLLOW sets:\n";
    for (const auto& entry : follow) {
        cout << "FOLLOW(" << entry.first << ") = { ";
        for (char c : entry.second) {
            cout << c << " ";
        }
        cout << "}\n";
    }
}

set<char> computeFirstOfString(const string& str) {
    set<char> result;
    if (str == EPSILON) {
        result.insert('#');
        return result;
    }

    bool allEpsilon = true;
    for (char symbol : str) {
        if (isNonTerminal(symbol)) {
            // Add all non-epsilon symbols from FIRST(symbol)
            for (char f : first[symbol]) {
                if (f != '#') {
                    result.insert(f);
                }
            }
            if (!first[symbol].count('#')) {
                allEpsilon = false;
                break;
            }
        } else {
            // Terminal symbol
            result.insert(symbol);
            allEpsilon = false;
            break;
        }
    }
    if (allEpsilon) {
        result.insert('#');
    }
    return result;
}

void generateParsingTable() {
    cout << "\nGenerating LL(1) Parsing Table...\n";
    parsingTable.clear();
    bool isLL1 = true;

    // First collect ALL non-terminals (including generated ones)
    set<char> allNonTerms;
    for (const auto& rule : rules) {
        allNonTerms.insert(rule.nonTerminal);
    }

    // Generate table entries
    for (size_t i = 0; i < rules.size(); i++) {
        const Rule& rule = rules[i];
        char N = rule.nonTerminal;

        for (size_t j = 0; j < rule.productions.size(); j++) {
            const string& prod = rule.productions[j];
            set<char> firstOfProd = computeFirstOfString(prod);

            for (char a : firstOfProd) {
                if (a != '#') {
                    if (parsingTable.find({N, a}) != parsingTable.end()) {
                        cerr << "Conflict detected for (" << N << ", " << a << ")\n";
                        isLL1 = false;
                    }
                    parsingTable[{N, a}] = {i, j};
                }
            }

            if (firstOfProd.count('#')) {
                for (char b : follow[N]) {
                    if (parsingTable.find({N, b}) != parsingTable.end()) {
                        cerr << "Conflict detected for (" << N << ", " << b << ")\n";
                        isLL1 = false;
                    }
                    parsingTable[{N, b}] = {i, j};
                }
            }
        }
    }

    if (!isLL1) {
        cout << "Grammar is not LL(1) due to conflicts listed above.\n";
        return;
    }

    // Display the table properly
    cout << "\nLL(1) Parsing Table:\n";
    
    // Get sorted terminals (only those actually in grammar)
    set<char> actualTerms;
    for (const auto& rule : rules) {
        for (const auto& prod : rule.productions) {
            for (char c : prod) {
                if (!isNonTerminal(c) && c != '#') {
                    actualTerms.insert(c);
                }
            }
        }
    }
    actualTerms.insert('+');  // Ensure + is included
    actualTerms.insert('$');  // Ensure $ is included

    // Print header
    cout << setw(10) << " ";
    for (char t : actualTerms) {
        cout << setw(15) << t;
    }
    cout << endl << string(10 + 15 * actualTerms.size(), '-') << endl;

    // Print rows for ALL non-terminals
    for (char nt : allNonTerms) {
        cout << setw(10) << nt;
        for (char t : actualTerms) {
            auto it = parsingTable.find({nt, t});
            if (it != parsingTable.end()) {
                const auto& [ruleIdx, prodIdx] = it->second;
                cout << setw(15) << (rules[ruleIdx].nonTerminal + "->" + rules[ruleIdx].productions[prodIdx]);
            } else {
                cout << setw(15) << " ";
            }
        }
        cout << endl;
    }
}

void parseInputString(const string& input) {
    cout << "\nParsing input: " << input << endl;
    
    stack<char> parseStack;
    parseStack.push('$');
    parseStack.push(start_symbol);
    
    vector<string> tokens;
    istringstream iss(input);
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    tokens.push_back("$"); // End marker
    
    size_t index = 0;
    int step = 1;
    
    cout << setw(5) << "Step" << setw(20) << "Stack" << setw(20) << "Input" << setw(30) << "Action" << endl;
    cout << string(75, '-') << endl;
    
    while (!parseStack.empty() && index < tokens.size()) {
        // Display current step
        cout << setw(5) << step++;
        
        // Display stack
        stack<char> temp = parseStack;
        string stackStr;
        while (!temp.empty()) {
            stackStr = string(1, temp.top()) + " " + stackStr;
            temp.pop();
        }
        cout << setw(20) << stackStr;
        
        // Display input
        string inputStr;
        for (size_t i = index; i < tokens.size(); i++) {
            inputStr += tokens[i] + " ";
        }
        cout << setw(20) << inputStr;
        
        char top = parseStack.top();
        string currentToken = tokens[index];
        char currentChar = currentToken[0]; // Simple token to char conversion
        
        if (top == currentChar) {
            // Match found
            cout << setw(30) << "Match " + string(1, top) << endl;
            parseStack.pop();
            index++;
        }
        else if (isNonTerminal(top)) {
            // Non-terminal - use parsing table
            auto it = parsingTable.find({top, currentChar});
            if (it != parsingTable.end()) {
                const auto& [ruleIdx, prodIdx] = it->second;
                const string& production = rules[ruleIdx].productions[prodIdx];
                
                cout << setw(30) << (string(1, top)) + "->" + production << endl;
                
                parseStack.pop();
                if (production != EPSILON) {
                    // Push production in reverse order
                    for (int i = production.length() - 1; i >= 0; i--) {
                        parseStack.push(production[i]);
                    }
                }
            }
            else {
                // Error - no production in table
                cout << setw(30) << "Error: No production for " + string(1, top) + " on " + currentToken << endl;
                return;
            }
        }
        else {
            // Error - mismatch
            cout << setw(30) << "Error: Expected " + string(1, top) + " but found " + currentToken << endl;
            return;
        }
    }
    
    if (parseStack.empty() && index == tokens.size()) {
        cout << "\nParsing completed successfully!\n";
    }
    else {
        cout << "\nParsing failed - input not fully consumed\n";
    }
}

void menu() {
    int choice;
    bool isFactored = false;
    bool isRecursionRemoved = false;
    do {
        cout << "\nSelect an action:\n";
        cout << "1. Display CFG\n";
        cout << "2. Perform Left Factoring\n";
        cout << "3. Perform Left Recursion Removal\n";
        cout << "4. Compute First Sets (Non-terminals only)\n";
        cout << "5. Compute Follow Sets\n";
        cout << "6. Generate LL(1) Parsing Table\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayCFG();
                break;
            case 2:
                performLeftFactoring();
                isFactored = true;
                break;
            case 3:
                performLeftRecursionRemoval();
                isRecursionRemoved = true;
                break;
            case 4:
                if (!isRecursionRemoved)
                {
                    cout << "Please perform Left Recursion Removal first.\n";
                    break;
                }
                computeFirstSets();
                break;
            case 5:
               if (!isRecursionRemoved)
                {
                    cout << "Please perform Left Recursion Removal first.\n";
                    break;
                }
                computeFollowSets();
                break;
            case 6:
                if (!isRecursionRemoved)
                {
                    cout << "Please perform Left Recursion Removal first.\n";
                    break;
                }
                if (!isFactored)
                {
                    cout << "Please perform Left Factoring first.\n";
                    break;
                }
                computeFirstSets();
                computeFollowSets();
                generateParsingTable();
                break;
            case 7:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 7);
}

int main() {
    string filename;
    cout << "Enter the name of the CFG file: ";
    cin >> filename;
    readCFGFromFile(filename);
    menu();
    ifstream inputFile("inputstring.txt");
    string line;
    while (getline(inputFile, line)) {
     parseInputString(line);
    }
    return 0;
}

