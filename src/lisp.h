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
};

symbol parse(int i, vector<pair<int, string>> v, vector<symbol> &p); // �������� ���� �м��� ����
symbol setq(int i, vector<pair<int, string>> v, vector<symbol> &p); // setq ���� ���� �м�
symbol quotation(int i, vector<pair<int, string>> v, vector<symbol> &p); // ' ���� ���� �м�

symbol parse(int i, vector<pair<int,string>> v, vector<symbol> &p) {
	symbol s;
	if (i == 0) { // () ���� �Ǵ�
		int a = 0, b = 0;
		for (int j = 0; j < v.size(); j++) {
			if (v[j].first == LEFT_PAREN) a++;
			else if (v[j].first == RIGHT_PAREN) b++;
		}
		if (a != b) {
			s.SetValue("error");
			return s;
		}
	}
	if (v[i].first == LEFT_PAREN) {
		if (v[i + 1].first == SETQ) {
			i++;
			s = setq(i, v, p);
			return s;
		} //���⿡ �ٸ� ��ɾ� �߰�
		else {
			s.SetIdent("");
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
			s.SetIdent("");
			s.SetValue(v[i + 1].second);
			return s;
		}
		else {
			s.SetIdent("");
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
		s.SetIdent("");
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
						s.SetIdent("");
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
					s.SetIdent("");
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
					s.SetIdent("");
					s.SetValue("error");
					return s;
				}
				else {
					s.SetIdent("");
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
				s.SetIdent("");
				s.SetValue("error");
				return s;
			}
		}
		else {
			s.SetIdent("");
			s.SetValue("error");
			return s;
		}
	}
	else {
		s.SetIdent("");
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
		bool check = false;
		for (; v[i].first != RIGHT_PAREN || check; i++) {
			if (i > v.size() - 1) {
				s.SetIdent("");
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
		s.SetIdent("");
		s.SetValue("error");
		return s;
	}
}