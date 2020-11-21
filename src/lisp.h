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
		if (index >= list.size() || index < 0) {
			symbol s;
			s.SetValue("error");
			return s;
		}
		return list[index];
	}
	void DeleteFromList(int index) { // list의 특정 index에 위치한 원소 제거
		if (index >= list.size() || index < 0) {
			cout << "error" << endl;
		}
		else {
			list.erase(list.begin() + index);
		}
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
	void Clear() {
		ident = "";
		value = "";
		list.clear();
		islist = false;
	}
};

symbol parse(int i, vector<pair<int, string>> v, vector<symbol> &p); // 전반적인 구문 분석을 수행
symbol setq(int i, vector<pair<int, string>> v, vector<symbol> &p); // setq 관련 구문 분석
symbol quotation(int i, vector<pair<int, string>> v, vector<symbol> &p); // ' 관련 구문 분석
symbol list(int i, vector<pair<int, string>> v, vector<symbol> &p); // list 관련 구문 분석
symbol arith_op(int i, vector<pair<int, string>> v, vector<symbol> &p); // 사칙연산 관련 구문 분석
symbol car(int i, vector<pair<int, string>> v, vector<symbol> &p); // car 관련 구문 분석
symbol cdr(int i, vector<pair<int, string>> v, vector<symbol> &p); // cdr 관련 구문 분석

symbol parse(int i, vector<pair<int,string>> v, vector<symbol> &p) {
	symbol s;
	if (i == 0) { // () 오류 판단
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
		} //여기에 다른 명령어 추가
		else {
			s.Clear();
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
		else if (v[i + 1].first == IDENT || v[i + 1].first == INT) {
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
	else if (v[i].first == IDENT) { // symbol 값 출력
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
		if (v[i].first == IDENT) {// symbol 중복 체크
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
			if (v[i].first == INT) { // (setq x number) 처리
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
		for (int j = i + 1; j < v.size(); j++) {
			if (v[j].first == IDENT || v[j].first == INT) { // symbol, number인 경우 list에 추가
				t.Clear();
				t.SetValue(v[j].second);
				s.AddList(t);
			}
			else if (v[j].first == LEFT_PAREN) { // (인 경우 다시 quotation 호출
				t = quotation(j, v, p);
				if (t.GetValue() == "error") return t;
				s.AddList(t);
				int temp = 0;
				for (int k = j; k < v.size() - 1; k++) { // () 위치 찾기
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
			else if (v[j].first == RIGHT_PAREN) { // 결과 return
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
		int check = 0;
		for (int j = i; j < v.size(); j++) { // 괄호 개수 확인
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
				for (int j = i + 1; j < v.size(); j++) { // () 닫히는 부분 찾기
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
				for (int j = i + 1; j < v.size(); j++) { // () 닫히는 부분 찾기
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
				for (int j = i + 1; j < v.size(); j++) { // () 닫히는 부분 찾기
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
				for (int j = i + 1; j < v.size(); j++) { // () 닫히는 부분 찾기
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
		if (v[i + 1].first == QUOTATION) { // (CAR '..) 처리
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
		else if (v[i + 1].first == IDENT) { // (CAR symbol) 처리
			i++;
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
		else if (v[i + 1].first == LEFT_PAREN) { // (CAR (...)) 처리
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
		if (v[i + 1].first == QUOTATION) { // (CDR '..) 처리
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
		else if (v[i + 1].first == IDENT) { // (CDR symbol) 처리
			i++;
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
		}
		else if (v[i + 1].first == LEFT_PAREN) { // (CDR (...)) 처리
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