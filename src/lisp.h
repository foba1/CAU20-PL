#include <iostream>
#include "lexer.h"

class symbol {
private:
	string ident;
	string value;
	vector<symbol> list;
	bool islist;
public:
	symbol() {
		ident = "";
		value = "";
		list.clear();
		islist = false;
	}
	void SetIdent(string str) { // ident setter
		ident = str;
	}
	string GetIdent() { // ident getter
		return ident;
	}
	void SetValue(string str) { // value setter
		value = str;
	}
	string GetValue() { // value getter
		return value;
	}
	bool IsList() { // islist getter
		return islist;
	}
	void AddList(symbol s) { // add symbol at list
		list.push_back(s);
		if (!islist) islist = true;
	}
	int GetListSize() { // size of list return
		return list.size();
	}
	symbol GetList(int index) { // list[index] return
		if (index >= list.size() || index < 0) {
			symbol s;
			s.SetValue("error");
			return s;
		}
		return list[index];
	}
	void DeleteFromList(int index) { // delete list[index]
		if (index >= list.size() || index < 0) {
			cout << "error" << endl;
		}
		else {
			list.erase(list.begin() + index);
		}
	}
	void PrintList() { // print list
		if (islist) {
			cout << "(";
			for (int i = 0; i < list.size(); i++) {
				list[i].PrintList();
				if (i != list.size() - 1) cout << " ";
			}
			cout << ")";
		}
		else {
			cout << value;
		}
	}
	void Clear() {
		ident = "";
		value = "";
		list.clear();
		islist = false;
	}
};

symbol parse(int i, vector<pair<int, string>> v, vector<symbol> &p); // parse
symbol setq(int i, vector<pair<int, string>> v, vector<symbol> &p); // setq
symbol quotation(int i, vector<pair<int, string>> v, vector<symbol> &p); // '
symbol list(int i, vector<pair<int, string>> v, vector<symbol> &p); // list
symbol arith_op(int i, vector<pair<int, string>> v, vector<symbol> &p); // + - * /
symbol car(int i, vector<pair<int, string>> v, vector<symbol> &p); // car
symbol cdr(int i, vector<pair<int, string>> v, vector<symbol> &p); // cdr
symbol cadr(int i, vector<pair<int, string>> v, vector<symbol> &p); //cadr
symbol nth(int i, vector<pair<int, string>> v, vector<symbol> &p); // nth
symbol reverse(int i, vector<pair<int, string>> v, vector<symbol> &p); // reverse

symbol parse(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s;
	if (i == 0) { // () error check
		int a = 0, b = 0;
		for (int j = 0; j < v.size(); j++) {
			if (v[j].first == LEFT_PAREN) a++;
			else if (v[j].first == RIGHT_PAREN) b++;
		}
		if (a != b) {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	if (v[i].first == LEFT_PAREN) {
		if (v[i + 1].first == SETQ) {
			i++;
			s = setq(i, v, p);
			return s;
		}
		else if (v[i + 1].first == LIST) {
			i++;
			s = list(i, v, p);
			return s;
		}
		else if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
			i++;
			s = arith_op(i, v, p);
			return s;
		}
		else if (v[i + 1].first == CAR) {
			i++;
			s = car(i, v, p);
			return s;
		}
		else if (v[i + 1].first == CDR) {
			i++;
			s = cdr(i, v, p);
			return s;
		}
		else if (v[i + 1].first == CADR) {
			i++;
			s = cadr(i, v, p);
			return s;
		}
		else if (v[i + 1].first == NTH) {
			i++;
			s = nth(i, v, p);
			return s;
		} // add another function here
		else if (v[i+1].first == REVERSE){
			i++;
			s = reverse(i,v,p);
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == QUOTATION) { // '
		if (v[i + 1].first == LEFT_PAREN) { // '(...)
			i++;
			s = quotation(i, v, p);
			return s;
		}
		else if (v[i + 1].first == IDENT || v[i + 1].first == INT) { // 'X or '1
			s.Clear();
			s.SetValue(v[i + 1].second);
			return s;
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == IDENT) { // if input is symbol, find at p[] and return it
		for (int j = 0; j < p.size(); j++) {
			if (p[j].GetIdent() == v[i].second) {
				s = p[j];
				return s;
			}
		}
		s.Clear();
		s.SetValue("error");
		return s;
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol setq(int i, vector<pair<int, string>> v, vector<symbol>& p) {
	symbol s;
	if (v[i].first == SETQ) {
		i++;
		if (v[i].first == IDENT) {// symbol overlap check
			if (p.size() == 0) {
				s.SetIdent(v[i].second);
			}
			else {
				s.SetIdent(v[i].second);
			}
			i++;
			if (v[i].first == INT) { // (setq x number)
				if (v[i + 1].first == RIGHT_PAREN) {
					s.SetValue(v[i].second);
					for (int j = 0; j < p.size(); j++) {
						if (p[j].GetIdent() == s.GetIdent()) {
							p.erase(p.begin() + j); // for overlap
						}
					}
					p.push_back(s);
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i].first == IDENT) { // (setq x symbol) 
				if (v[i + 1].first == RIGHT_PAREN) {
					if (v[i].second == v[i + 1].second) { // (setq X X) error
						s.Clear();
						s.SetValue("error");
						return s;
					}
					for (int k = 0; k < p.size(); k++) {
						if (v[i].second == p[k].GetIdent()) {
							if (p[k].IsList()) {
								symbol t = p[k];
								t.SetIdent(s.GetIdent());
								p.push_back(t);
								return t;
							}
							else {
								s.SetValue(p[k].GetValue());
								p.push_back(s);
								return s;
							}
						}
					}
					s.Clear();
					s.SetValue("error");
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i].first == QUOTATION) { // (setq x '..)
				symbol t;
				t = parse(i, v, p);
				if (t.GetValue() == "error") {
					return t;
				}
				t.SetIdent(s.GetIdent());
				for (int j = 0; j < p.size(); j++) {
					if (p[j].GetIdent() == t.GetIdent()) {
						p.erase(p.begin() + j); // for overlap
					}
				}
				p.push_back(t);
				return t;
			}
			else if (v[i].first == LEFT_PAREN) { // (setq x (...))
				symbol t;
				t = parse(i, v, p);
				if (t.GetValue() == "error") {
					return t;
				}
				t.SetIdent(s.GetIdent());
				for (int j = 0; j < p.size(); j++) {
					if (p[j].GetIdent() == t.GetIdent()) {
						p.erase(p.begin() + j); // for overlap
					}
				}
				p.push_back(t);
				return t;
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol quotation(int i, vector<pair<int, string>> v, vector<symbol>& p) {
	symbol s, t;
	if (v[i].first == LEFT_PAREN) {
		if (v[i + 1].first == RIGHT_PAREN) {
			s.Clear();
			s.SetValue("NIL");
			return s;
		}
		for (int j = i + 1; j < v.size(); j++) {
			if (v[j].first == IDENT || v[j].first == INT) { // add symbol or numver to list
				t.Clear();
				t.SetValue(v[j].second);
				s.AddList(t);
			}
			else if (v[j].first == LEFT_PAREN) { // (
				t = quotation(j, v, p);
				if (t.GetValue() == "error") return t;
				s.AddList(t);
				int temp = 0;
				for (int k = j; k < v.size() - 1; k++) { // find )
					if (v[k].first == LEFT_PAREN) {
						temp++;
						continue;
					}
					else if (v[k].first == RIGHT_PAREN && temp > 0) temp--;
					if (temp == 0) {
						j = k;
						break;
					}
				}
			}
			else if (v[j].first == RIGHT_PAREN) { // result return
				return s;
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol list(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s, t;
	if (v[i].first == LIST) {
		i++;
		for (int j = i; j < v.size(); j++) {
			if (v[j].first == RIGHT_PAREN) break;
			else if (v[j].first == QUOTATION) { // (list '...)
				t = parse(j, v, p);
				int check = 0;
				for (int k = j + 1; k < v.size(); k++) { // find )
					if (v[k].first == LEFT_PAREN) check++;
					else if (v[k].first == RIGHT_PAREN && check > 0) check--;
					if (check == 0) {
						j = k;
						break;
					}
				}
				s.AddList(t);
			}
			else if (v[j].first == IDENT) { // (list symbol)
				for (int k = 0; k < p.size(); k++) {
					if (p[k].GetIdent() == v[j].second) {
						if (p[k].IsList()) {
							t = p[k];
							t.SetIdent("");
							s.AddList(t);
						}
						else {
							t.Clear();
							t.SetValue(p[k].GetValue());
							s.AddList(t);
						}
					}
				}
				t.Clear();
				t.SetValue("error");
				return t;
			}
			else if (v[j].first == INT) { // (list number)
				t.Clear();
				t.SetValue(v[j].second);
				s.AddList(t);
			}
			else {
				t.Clear();
				t.SetValue("error");
				return t;
			}
		}
		return s;
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol arith_op(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s;
	int a, b;
	if (v[i].first == ADD_OP) { // +
		i++;
		if (v[i].first == INT) {
			if (v[i + 1].first == INT) {
				if (v[i + 2].first == RIGHT_PAREN) {
					a = stoi(v[i].second);
					b = stoi(v[i + 1].second);
					s.Clear();
					s.SetValue(to_string(a + b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i + 1].first == LEFT_PAREN) {
				a = stoi(v[i].second);
				if (v[i + 2].first == ADD_OP || v[i + 2].first == SUB_OP || v[i + 2].first == MUL_OP || v[i + 2].first == DIV_OP) {
					s = parse(i + 1, v, p);
					if (s.GetValue() != "error") {
						b = stoi(s.GetValue());
					}
					s.Clear();
					s.SetValue(to_string(a + b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else if (v[i].first == LEFT_PAREN) {
			if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
				s = parse(i, v, p);
				if (s.GetValue() != "error") {
					a = stoi(s.GetValue());
				}
				int num = 1;
				for (int j = i + 1; j < v.size(); j++) { // find )
					if (v[j].first == LEFT_PAREN) num++;
					else if (v[j].first == RIGHT_PAREN && num > 0) num--;
					if (num == 0) {
						i = j + 1;
						break;
					}
				}
				if (v[i].first == INT) {
					if (v[i + 1].first == RIGHT_PAREN) {
						b = stoi(v[i].second);
						s.Clear();
						s.SetValue(to_string(a + b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else if (v[i].first == LEFT_PAREN) {
					if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
						s = parse(i, v, p);
						if (s.GetValue() != "error") {
							b = stoi(s.GetValue());
						}
						s.Clear();
						s.SetValue(to_string(a + b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == SUB_OP) { // -
		i++;
		if (v[i].first == INT) {
			if (v[i + 1].first == INT) {
				if (v[i + 2].first == RIGHT_PAREN) {
					a = stoi(v[i].second);
					b = stoi(v[i + 1].second);
					s.Clear();
					s.SetValue(to_string(a - b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i + 1].first == LEFT_PAREN) {
				a = stoi(v[i].second);
				if (v[i + 2].first == ADD_OP || v[i + 2].first == SUB_OP || v[i + 2].first == MUL_OP || v[i + 2].first == DIV_OP) {
					s = parse(i + 1, v, p);
					if (s.GetValue() != "error") {
						b = stoi(s.GetValue());
					}
					s.Clear();
					s.SetValue(to_string(a - b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else if (v[i].first == LEFT_PAREN) {
			if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
				s = parse(i, v, p);
				if (s.GetValue() != "error") {
					a = stoi(s.GetValue());
				}
				int num = 1;
				for (int j = i + 1; j < v.size(); j++) { // find )
					if (v[j].first == LEFT_PAREN) num++;
					else if (v[j].first == RIGHT_PAREN && num > 0) num--;
					if (num == 0) {
						i = j + 1;
						break;
					}
				}
				if (v[i].first == INT) {
					if (v[i + 1].first == RIGHT_PAREN) {
						b = stoi(v[i].second);
						s.Clear();
						s.SetValue(to_string(a - b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else if (v[i].first == LEFT_PAREN) {
					if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
						s = parse(i, v, p);
						if (s.GetValue() != "error") {
							b = stoi(s.GetValue());
						}
						s.Clear();
						s.SetValue(to_string(a - b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == MUL_OP) { // *
		i++;
		if (v[i].first == INT) {
			if (v[i + 1].first == INT) {
				if (v[i + 2].first == RIGHT_PAREN) {
					a = stoi(v[i].second);
					b = stoi(v[i + 1].second);
					s.Clear();
					s.SetValue(to_string(a * b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i + 1].first == LEFT_PAREN) {
				a = stoi(v[i].second);
				if (v[i + 2].first == ADD_OP || v[i + 2].first == SUB_OP || v[i + 2].first == MUL_OP || v[i + 2].first == DIV_OP) {
					s = parse(i + 1, v, p);
					if (s.GetValue() != "error") {
						b = stoi(s.GetValue());
					}
					s.Clear();
					s.SetValue(to_string(a * b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else if (v[i].first == LEFT_PAREN) {
			if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
				s = parse(i, v, p);
				if (s.GetValue() != "error") {
					a = stoi(s.GetValue());
				}
				int num = 1;
				for (int j = i + 1; j < v.size(); j++) { // find )
					if (v[j].first == LEFT_PAREN) num++;
					else if (v[j].first == RIGHT_PAREN && num > 0) num--;
					if (num == 0) {
						i = j + 1;
						break;
					}
				}
				if (v[i].first == INT) {
					if (v[i + 1].first == RIGHT_PAREN) {
						b = stoi(v[i].second);
						s.Clear();
						s.SetValue(to_string(a * b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else if (v[i].first == LEFT_PAREN) {
					if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
						s = parse(i, v, p);
						if (s.GetValue() != "error") {
							b = stoi(s.GetValue());
						}
						s.Clear();
						s.SetValue(to_string(a * b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == DIV_OP) { // /
		i++;
		if (v[i].first == INT) {
			if (v[i + 1].first == INT) {
				if (v[i + 2].first == RIGHT_PAREN) {
					a = stoi(v[i].second);
					b = stoi(v[i + 1].second);
					s.Clear();
					s.SetValue(to_string(a / b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i + 1].first == LEFT_PAREN) {
				a = stoi(v[i].second);
				if (v[i + 2].first == ADD_OP || v[i + 2].first == SUB_OP || v[i + 2].first == MUL_OP || v[i + 2].first == DIV_OP) {
					s = parse(i + 1, v, p);
					if (s.GetValue() != "error") {
						b = stoi(s.GetValue());
					}
					s.Clear();
					s.SetValue(to_string(a / b));
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else if (v[i].first == LEFT_PAREN) {
			if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
				s = parse(i, v, p);
				if (s.GetValue() != "error") {
					a = stoi(s.GetValue());
				}
				int num = 1;
				for (int j = i + 1; j < v.size(); j++) { // find )
					if (v[j].first == LEFT_PAREN) num++;
					else if (v[j].first == RIGHT_PAREN && num > 0) num--;
					if (num == 0) {
						i = j + 1;
						break;
					}
				}
				if (v[i].first == INT) {
					if (v[i + 1].first == RIGHT_PAREN) {
						b = stoi(v[i].second);
						s.Clear();
						s.SetValue(to_string(a / b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else if (v[i].first == LEFT_PAREN) {
					if (v[i + 1].first == ADD_OP || v[i + 1].first == SUB_OP || v[i + 1].first == MUL_OP || v[i + 1].first == DIV_OP) {
						s = parse(i, v, p);
						if (s.GetValue() != "error") {
							b = stoi(s.GetValue());
						}
						s.Clear();
						s.SetValue(to_string(a / b));
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol car(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s;
	if (v[i].first == CAR) {
		if (v[i + 1].first == QUOTATION) { // (CAR '..)
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			else if (!s.IsList() || s.GetListSize() == 0) {
				s.Clear();
				s.SetValue("NIL");
				return s;
			}
			else {
				return s.GetList(0);
			}
		}
		else if (v[i + 1].first == IDENT) { // (CAR symbol)
			i++;
			if (v[i + 1].first != RIGHT_PAREN) { // () error check
				s.Clear();
				s.SetValue("error");
				return s;
			}
			for (int j = 0; j < p.size(); j++) {
				if (v[i].second == p[j].GetIdent()) {
					s = p[j];
					if (!s.IsList() || s.GetListSize() == 0) {
						s.Clear();
						s.SetValue("NIL");
						return s;
					}
					else {
						return s.GetList(0);
					}
				}
			}
			s.Clear();
			s.SetValue("error");
			return s;
		}
		else if (v[i + 1].first == LEFT_PAREN) { // (CAR (...))
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			int temp = 0;
			for (int k = i; k < v.size() - 1; k++) { // find )
				if (v[k].first == LEFT_PAREN) {
					temp++;
					continue;
				}
				else if (v[k].first == RIGHT_PAREN && temp > 0) temp--;
				if (temp == 0) {
					i = k + 1;
					break;
				}
			}
			if (v[i].first == RIGHT_PAREN) {
				if (!s.IsList() || s.GetListSize() == 0) {
					s.Clear();
					s.SetValue("NIL");
					return s;
				}
				else {
					return s.GetList(0);
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol cdr(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s;
	if (v[i].first == CDR) {
		if (v[i + 1].first == QUOTATION) { // (CDR '..)
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			else if (!s.IsList() || s.GetListSize() <= 1) {
				s.Clear();
				s.SetValue("NIL");
				return s;
			}
			else {
				s.DeleteFromList(0);
				return s;
			}
		}
		else if (v[i + 1].first == IDENT) { // (CDR symbol)
			i++;
			if (v[i + 1].first != RIGHT_PAREN) { // () error check
				s.Clear();
				s.SetValue("error");
				return s;
			}
			for (int j = 0; j < p.size(); j++) {
				if (v[i].second == p[j].GetIdent()) {
					s = p[j];
					if (!s.IsList() || s.GetListSize() <= 1) {
						s.Clear();
						s.SetValue("NIL");
						return s;
					}
					else {
						s.DeleteFromList(0);
						return s;
					}
				}
			}
			s.Clear();
			s.SetValue("error");
			return s;
		}
		else if (v[i + 1].first == LEFT_PAREN) { // (CDR (...))
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			int temp = 0;
			for (int k = i; k < v.size() - 1; k++) { // find )
				if (v[k].first == LEFT_PAREN) {
					temp++;
					continue;
				}
				else if (v[k].first == RIGHT_PAREN && temp > 0) temp--;
				if (temp == 0) {
					i = k + 1;
					break;
				}
			}
			if (v[i].first == RIGHT_PAREN) {
				if (!s.IsList() || s.GetListSize() <= 1) {
					s.Clear();
					s.SetValue("NIL");
					return s;
				}
				else {
					s.DeleteFromList(0);
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol cadr(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s;
	if (v[i].first == CADR) {
		if (v[i + 1].first == QUOTATION) { // (CADR '..)
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			else if (!s.IsList() || s.GetListSize() <= 1) {
				s.Clear();
				s.SetValue("NIL");
				return s;
			}
			else {
				return s.GetList(v[i - 1].second.length() - 3); //return d's number
			}
		}
		else if (v[i + 1].first == IDENT) { // (CADR symbol)
			i++;
			if (v[i + 1].first != RIGHT_PAREN) { // () error check
				s.Clear();
				s.SetValue("error");
				return s;
			}
			for (int j = 0; j < p.size(); j++) {
				if (v[i].second == p[j].GetIdent()) {
					s = p[j];
					if (!s.IsList() || s.GetListSize() <= 1) {
						s.Clear();
						s.SetValue("NIL");
						return s;
					}
					else {
						return s.GetList(v[i - 1].second.length() - 3); //return d's number
					}
				}
			}
			s.Clear();
			s.SetValue("error");
			return s;
		}
		else if (v[i + 1].first == LEFT_PAREN) { // (CDR (...))
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			int temp = 0;
			for (int k = i; k < v.size() - 1; k++) { // find )
				if (v[k].first == LEFT_PAREN) {
					temp++;
					continue;
				}
				else if (v[k].first == RIGHT_PAREN && temp > 0) temp--;
				if (temp == 0) {
					i = k + 1;
					break;
				}
			}
			if (v[i].first == RIGHT_PAREN) { //)
				if (!s.IsList() || s.GetListSize() <= 1) {
					s.Clear();
					s.SetValue("NIL");
					return s;
				}
				else {
					return s.GetList(v[i - 1].second.length() - 3); //return d's number
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}

symbol nth(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s;
	string output;
	int n, count = 0;
	if (v[i].first == NTH) {
		if (v[i + 1].first == INT) {
			i++;
			n = stoi(v[i].second);
			if (v[i + 1].first == QUOTATION) {
				i++;
				if (v[i + 1].first == LEFT_PAREN) {
					i++;
					int leftcount = 1;
					while (leftcount && count < n) {
						if (v[i + 1].first == INT || v[i + 1].first == IDENT) {
							i++;
							count++;
						}
						else if (v[i + 1].first == LEFT_PAREN) {
							leftcount++;
							i++;
							while (leftcount > 1) {
								if (v[i + 1].first == LEFT_PAREN)
									leftcount++;
								else if (v[i + 1].first == RIGHT_PAREN)
									leftcount--;
								i++;
							}
							i++;
							count++;
						}
						else if (v[i + 1].first == RIGHT_PAREN) {
							leftcount--;
							i++;
						}
						else {
							s.Clear();
							s.SetValue("error");
							return s;
						}
					}//이 밑으로 cadr 만들어지면 그걸로 대체해도 될 듯
					if (count < n) {
						s.Clear();
						s.SetValue("error");
						return s;
					}
					else if (v[i + 1].first == RIGHT_PAREN) {
						s.Clear();
						s.SetValue("NIL");
						return s;
					}
					else if (v[i + 1].first == INT || v[i + 1].first == IDENT) {
						s.Clear();
						s.SetValue(v[i + 1].second);
						return s;
					}
					else if (v[i + 1].first == LEFT_PAREN) {
						output += "( ";
						leftcount++;
						i++;
						while (leftcount > 1) {
							if (v[i + 1].first == LEFT_PAREN)
								leftcount++;
							else if (v[i + 1].first == RIGHT_PAREN)
								leftcount--;
							else if (v[i + 1].first == INT || v[i + 1].first == IDENT)
								leftcount;
							else {
								s.Clear();
								s.SetValue("error");
								return s;
							}
							output += v[i + 1].second + " ";
							i++;
						}
						s.Clear();
						s.SetValue(output);
						return s;
					}
					else {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}
symbol reverse(int i, vector<pair<int, string>> v, vector<symbol> &p){
	symbol s;
	string temp;
	int left,right;
	if (v[i].first == REVERSE) {
		if (v[i + 1].first == QUOTATION) { // EX_ (REVERSE '(1 2 3 4))
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			else if (!s.IsList() || s.GetListSize() < 1) {
				s.Clear();
				s.SetValue("NIL");
				return s;
			}
			else {//밑과 같은 방식X 
				left = 0, right = s.GetListSize()-1;
				for(int x=0;x<s.GetListSize()/2;x++){
					temp = v[i+2+left].second;
					v[i+2+left].second = v[i+2+right].second;
					v[i+2+right].second = temp;
					left++;right--;
				}
				for(int x=0;x<s.GetListSize();x++){
					cout <<v[i+2+x].second;
					
					cout << s.GetList(s.GetListSize()-x-1).GetValue();
				}
				cout << endl;
				return s;
			}
		}
		else if (v[i + 1].first == IDENT) { // (CADR symbol)
			i++;
			if (v[i + 1].first != RIGHT_PAREN) { // () error check
				s.Clear();
				s.SetValue("error");
				return s;
			}
			for (int j = 0; j < p.size(); j++) {
				if (v[i].second == p[j].GetIdent()) {
					s = p[j];
					if (!s.IsList() || s.GetListSize() <= 1) {
						s.Clear();
						s.SetValue("NIL");
						return s;
					}
					else {
						return s.GetList(v[i - 1].second.length() - 3); //return d's number
					}
				}
			}
			s.Clear();
			s.SetValue("error");
			return s;
		}
		else if (v[i + 1].first == LEFT_PAREN) { // (CDR (...))
			i++;
			s = parse(i, v, p);
			if (s.GetValue() == "error") return s;
			int temp = 0;
			for (int k = i; k < v.size() - 1; k++) { // find )
				if (v[k].first == LEFT_PAREN) {
					temp++;
					continue;
				}
				else if (v[k].first == RIGHT_PAREN && temp > 0) temp--;
				if (temp == 0) {
					i = k + 1;
					break;
				}
			}
			if (v[i].first == RIGHT_PAREN) { //)
				if (!s.IsList() || s.GetListSize() <= 1) {
					s.Clear();
					s.SetValue("NIL");
					return s;
				}
				else {
					return s.GetList(v[i - 1].second.length() - 3); //return d's number
				}
			}
			else {
				s.Clear();
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.Clear();
		s.SetValue("error");
		return s;
	}
}