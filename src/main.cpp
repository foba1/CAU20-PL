#include <iostream>
#include "lexer.h"
#include "lisp.h"

using namespace std;

int main() {
	lexer Lexer; // Initialize lexer
	vector<pair<int, string>> v; // Save values returned by lexer.h
	vector<symbol> s; // Save symbol variables
	string str; // Input string
	symbol t; // Result symbol
	while (1) { // Repeat until get input 0
		cout << "Enter the string (exit : 0) : ";
		getline(cin, str);
		if (str == "0") break;
		else if (str == "") continue; // Ignore input '\n'
		v = Lexer.lex(str);
		t = parse(0, v, s); // Parse and get result symbol
		if (t.GetValue() == "error") {
			cout << "error" << endl;
		}
		else {
			if (t.IsList()) {
				t.PrintList();
				cout << endl;
			}
			else {
				cout << t.GetValue() << endl;
			}
		}
	}
	return 0;
}