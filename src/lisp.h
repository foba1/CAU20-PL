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
	void AddList(symbol s) { // list�� symbol �߰�
		list.push_back(s);
		if (!islist) islist = true;
	}
	int GetListSize() { // list�� ũ�� return
		return list.size();
	}
	symbol GetList(int index) { // list�� Ư�� symbol return
		return list[index];
	}
	void PrintList() { // list ���
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

symbol parse(int i, vector<pair<int, string>> v, vector<symbol> &p); // �������� ���� �м��� ����
symbol setq(int i, vector<pair<int, string>> v, vector<symbol> &p); // setq ���� ���� �м�
symbol quotation(int i, vector<pair<int, string>> v, vector<symbol> &p); // ' ���� ���� �м�
symbol list(int i, vector<pair<int, string>> v, vector<symbol> &p); // list ���� ���� �м�
symbol arith_op(int i, vector<pair<int, string>> v, vector<symbol> &p); // ��Ģ���� ���� ���� �м�

symbol parse(int i, vector<pair<int,string>> v, vector<symbol> &p) {
	symbol s;
	if (i == 0) { // () ���� �Ǵ�
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
		} //���⿡ �ٸ� ��ɾ� �߰�
		else {
			s.Clear();
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == QUOTATION) { // '�ڿ� ������ ���� ó�� - '(a b), 'X ���� ��
		if (v[i + 1].first == LEFT_PAREN) {
			i++;
			s = quotation(i, v, p);
			return s;
		}
		else if (v[i + 1].first == IDENT) {
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
	else if (v[i].first == IDENT) { // symbol �� ���
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
}

symbol setq(int i, vector<pair<int, string>> v, vector<symbol>& p) {
	symbol s;
	if (v[i].first == SETQ) {
		i++;
		if (v[i].first == IDENT) {// symbol �ߺ� üũ
			if (p.size() == 0) {
				s.SetIdent(v[i].second);
			}
			else {
				for (int j = 0; j < p.size(); j++) {
					if (p[j].GetIdent() == v[i].second) {
						s.Clear();
						s.SetValue("error");
						return s;
					}
				}
				s.SetIdent(v[i].second);
			}
			i++;
			if (v[i].first == INT) { // (setq x number) ó��
				if (v[i + 1].first == RIGHT_PAREN) {
					s.SetValue(v[i].second);
					p.push_back(s);
					return s;
				}
				else {
					s.Clear();
					s.SetValue("error");
					return s;
				}
			}
			else if (v[i].first == IDENT) { // (setq x symbol) ó��
				if (v[i + 1].first == RIGHT_PAREN) {
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
			else if (v[i].first == QUOTATION) { // (setq x '..) ó��
				symbol t;
				t = parse(i, v, p);
				if (t.GetValue() == "error") {
					return t;
				}
				t.SetIdent(s.GetIdent());
				p.push_back(t);
				return t;
			}
			else if (v[i].first == LEFT_PAREN) { // (setq x (...)) ó��
				symbol t;
				t = parse(i, v, p);
				if (t.GetValue() == "error") {
					return t;
				}
				t.SetIdent(s.GetIdent());
				p.push_back(t);
				s.SetIdent(to_string(i));
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
	if (v[i].first == IDENT) { // symbol ó��
		t.SetValue(v[i].second);
		return t;
	}
	else if (v[i].first == INT) { // number ó��
		t.SetValue(v[i].second);
		return t;
	}
	else if (v[i].first == LEFT_PAREN) { // () ó��
		i++;
		if (v[i].first == RIGHT_PAREN) {
			t.Clear();
			t.SetValue("NIL");
			return t;
		}
		bool check = false;
		for (; v[i].first != RIGHT_PAREN || check; i++) {
			if (i > v.size() - 1) {
				s.Clear();
				s.SetValue("error");
				return s;
			}
			if (!check) {
				t = quotation(i, v, p);
				if (t.GetValue() == "error") return t;
				s.AddList(t);
			}
			if (v[i].first == LEFT_PAREN) check = true;
			else if (v[i].first == RIGHT_PAREN && check) {
				check = false;
				continue;
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

symbol list(int i, vector<pair<int, string>> v, vector<symbol> &p) {
	symbol s, t;
	if (v[i].first == LIST) {
		i++;
		int check = 0; // ��ȣ ���� Ȯ��
		for (int j = i; j < v.size(); j++) {
			if (v[j].first == RIGHT_PAREN) break;
			else if (v[j].first == QUOTATION) {
				t = parse(j, v, p);
				for (int k = j + 1; k < v.size(); k++) {
					if (v[k].first == LEFT_PAREN) check++;
					else if (v[k].first == RIGHT_PAREN && check > 0) check--;

					if (check == 0) {
						j = k;
						break;
					}
				}
				s.AddList(t);
			}
			else if (v[j].first == IDENT) {
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
			else if (v[j].first == INT) {
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
	if (v[i].first == ADD_OP) {
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
				for (int j = i + 1; j < v.size(); j++) { // () ������ �κ� ã��
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
	else if (v[i].first == SUB_OP) {
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
				for (int j = i + 1; j < v.size(); j++) { // () ������ �κ� ã��
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
	else if (v[i].first == MUL_OP) {
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
				for (int j = i + 1; j < v.size(); j++) { // () ������ �κ� ã��
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
	else if (v[i].first == DIV_OP) {
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
				for (int j = i + 1; j < v.size(); j++) { // () ������ �κ� ã��
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