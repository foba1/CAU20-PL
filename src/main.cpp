#include <iostream>
#include "lexer.h"
#include "lisp.h"

using namespace std;

int main() {
	lexer Lexer;
	vector<pair<int, string>> v;
	vector<symbol> s;
	vector<symbol> &p = s;
	string str;
	symbol t;
	while (1) {
		cout << "Enter the string (exit : 0) : ";
		getline(cin, str);
		if (str == "0") break;
		else if (str == "") continue;
		v = Lexer.lex(str);
		t = parse(0, v, p);
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
		/* �׽�Ʈ (����� �ɺ��� ��� ����ϴ� ���)
		cout << "symbol list" << endl;
		for (int j = 0; j < s.size(); j++) {
			if (s[j].GetValue() == "error") {
				cout << "error" << endl;
			}
			else {
				if (s[j].IsList()) {
					s[j].PrintList();
					cout << endl;
				}
				else {
					cout << s[j].GetValue() << endl;
				}
			}
			
		}
		*/
	}
	return 0;
}