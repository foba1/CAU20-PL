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
	void AddList(symbol s) { // list에 symbol 추가
		list.push_back(s);
		if (!islist) islist = true;
	}
	int GetListSize() { // list의 크기 return
		return list.size();
	}
	symbol GetList(int index) { // list의 특정 symbol return
		return list[index];
	}
	void PrintList() { // list 출력
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

symbol parse(int i, vector<pair<int, string>> v, vector<symbol> &p); // 전반적인 구문 분석을 수행
symbol setq(int i, vector<pair<int, string>> v, vector<symbol> &p); // setq 관련 구문 분석
symbol quotation(int i, vector<pair<int, string>> v, vector<symbol> &p); // ' 관련 구문 분석

symbol parse(int i, vector<pair<int,string>> v, vector<symbol> &p) {
	symbol s;
	if (i == 0) { // () 오류 판단
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
		} //여기에 다른 명령어 추가
		else {
			s.SetIdent("");
			s.SetValue("error");
			return s;
		}
	}
	else if (v[i].first == QUOTATION) { // '뒤에 나오는 구문 처리 - '(a b), 'X 같은 것
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
	else if (v[i].first == IDENT) { // symbol 값 출력
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
		if (v[i].first == IDENT) {// symbol 중복 체크
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
			if (v[i].first == INT) { // (setq x number) 처리
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
			else if (v[i].first == IDENT) { // (setq x symbol) 처리
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
			else if (v[i].first == QUOTATION) { // (setq x '..) 처리
				symbol t;
				t = parse(i, v, p);
				if (t.GetValue() == "error") {
					return t;
				}
				t.SetIdent(s.GetIdent());
				p.push_back(t);
				return t;
			}
			else if (v[i].first == LEFT_PAREN) { // (setq x (...)) 처리
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
	if (v[i].first == IDENT) { // symbol 처리
		t.SetValue(v[i].second);
		return t;
	}
	else if (v[i].first == INT) { // number 처리
		t.SetValue(v[i].second);
		return t;
	}
	else if (v[i].first == LEFT_PAREN) { // () 처리
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