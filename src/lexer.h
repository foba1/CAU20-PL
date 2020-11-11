#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>

using namespace std;

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN -1
#define EOF 2

/* Token codes */
#define IDENT 10
#define INT 11
#define D_QUOTATION 12
#define LEFT_PAREN 13
#define RIGHT_PAREN 14
#define ADD_OP 15
#define SUB_OP 16
#define MUL_OP 17
#define DIV_OP 18
#define SEMICOLON 19
#define QUOTATION 20
#define NIL 21
#define SETQ 22
#define LIST 23
#define CAR 24
#define CDR 25
#define CADR 26
#define NTH 27
#define CONS 28
#define REVERSE 29
#define APPEND 30
#define LENGTH 31
#define MEMBER 32
#define ASSOC 33
#define REMOVE 34
#define SUBST 35
#define ATOM 36
#define NULL 37
#define NUMBERP 38
#define ZEROP 39
#define MINUSP 40
#define EQUAL 41
#define UNDER 42
#define UPTO 43
#define OVER 44
#define DOWNTO 45
#define STRINGP 46
#define IF 47
#define COND 48

class lexer {
private:
	vector<pair<int, string>> v;
	pair<int, string> p;
	string str, lexeme;
	int charCLass = 0;
	void getChar(int index) {
		if (str[index] != '\n') {
			if (isalpha(str[index])) {
				charCLass = LETTER;
			}
			else if (isdigit(str[index])) {
				charCLass = DIGIT;
			}
			else {
				charCLass = UNKNOWN;
			}
		}
		else {
			charCLass = EOF;
		}
	}
	void addChar(int index) {
		char temp;
		if (str[index] >= 'a' && str[index] <= 'z') {
			temp = str[index] - 32;
		}
		else {
			temp = str[index];
		}
		lexeme = lexeme + temp;
	}
	void lookup(int& index) {
		addChar(index);
		switch (str[index]) {
		case '"':
			p.first = D_QUOTATION;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '(':
			p.first = LEFT_PAREN;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case ')':
			p.first = RIGHT_PAREN;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '+':
			p.first = ADD_OP;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '-':
			p.first = SUB_OP;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '*':
			p.first = MUL_OP;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '/':
			p.first = DIV_OP;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case ';':
			p.first = SEMICOLON;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '\'':
			p.first = QUOTATION;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		case '<':
			getChar(index + 1);
			if (charCLass == UNKNOWN) {
				addChar(index + 1);
				if (lexeme == "<=") {
					p.first = UPTO;
					p.second = lexeme;
					this->v.push_back(p);
					index++;
				}
				else {
					p.first = UNDER;
					p.second = "<";
					this->v.push_back(p);
				}
			}
			else {
				p.first = UNDER;
				p.second = lexeme;
				this->v.push_back(p);
			}
			break;
		case '>':
			getChar(index + 1);
			if (charCLass == UNKNOWN) {
				addChar(index + 1);
				if (lexeme == ">=") {
					p.first = DOWNTO;
					p.second = lexeme;
					this->v.push_back(p);
					index++;
				}
				else {
					p.first = OVER;
					p.second = ">";
					this->v.push_back(p);
				}
			}
			else {
				p.first = OVER;
				p.second = lexeme;
				this->v.push_back(p);
			}
			break;
		default:
			p.first = UNKNOWN;
			p.second = lexeme;
			this->v.push_back(p);
			break;
		}
	}
public:
	lexer() {
		v.clear();
		str = "";
	}
	vector<pair<int, string>> lex(string s) {
		v.clear();
		str = s;
		bool check;
		for (int i = 0; i < s.length(); i++) {
			if (str[i] == ' ') continue;
			lexeme = "";
			getChar(i);
			int j = i;
			check = false;
			int& temp = i;
			switch (charCLass) {
				case LETTER:
					addChar(i);
					getChar(++j);
					while (charCLass == LETTER || charCLass == DIGIT) {
						if (charCLass == DIGIT) check = true;
						addChar(++i);
						getChar(++j);
					}
					if (!check) {
						if (lexeme == "NIL") {
							p.first = NIL;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "SETQ") {
							p.first = SETQ;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "LIST") {
							p.first = LIST;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "CAR") {
							p.first = CAR;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "CDR") {
							p.first = CDR;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme[0] == 'C' && lexeme[1] == 'A' && lexeme[2] == 'D') {
							for (int k = 3; k < lexeme.length(); k++) {
								if (lexeme[k] == 'D') {
									if (k == lexeme.length() - 1) {
										p.first = IDENT;
										p.second = lexeme;
										v.push_back(p);
									}
									else continue;
								}
								else if (lexeme[k] == 'R' && k == lexeme.length() - 1) {
									p.first = CADR;
									p.second = lexeme;
									v.push_back(p);
								}
								else {
									p.first = IDENT;
									p.second = lexeme;
									v.push_back(p);
								}
							}
						}
						else if (lexeme == "NTH") {
							p.first = NTH;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "CONS") {
							p.first = CONS;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "REVERSE") {
							p.first = REVERSE;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "APPEND") {
							p.first = APPEND;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "LENGTH") {
							p.first = LENGTH;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "MEMBER") {
							p.first = MEMBER;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "ASSOC") {
							p.first = ASSOC;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "REMOVE") {
							p.first = REMOVE;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "SUBST") {
							p.first = SUBST;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "ATOM") {
							p.first = ATOM;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "NULL") {
							p.first = NULL;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "NUMBERP") {
							p.first = NUMBERP;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "ZEROP") {
							p.first = ZEROP;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "MINUSP") {
							p.first = MINUSP;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "EQUAL") {
							p.first = EQUAL;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "STRINGP") {
							p.first = STRINGP;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "IF") {
							p.first = IF;
							p.second = lexeme;
							v.push_back(p);
						}
						else if (lexeme == "COND") {
							p.first = COND;
							p.second = lexeme;
							v.push_back(p);
						}
						else {
							p.first = IDENT;
							p.second = lexeme;
							v.push_back(p);
						}
					}
					else {
						p.first = IDENT;
						p.second = lexeme;
						v.push_back(p);
					}
					break;
				case DIGIT:
					addChar(i);
					getChar(++j);
					while (charCLass == DIGIT) {
						addChar(++i);
						getChar(++j);
					}
					p.first = INT;
					p.second = lexeme;
					v.push_back(p);
					break;
				case UNKNOWN:
					lookup(temp);
					break;
				case EOF:
					p.first = EOF;
					p.second = "\n";
					v.push_back(p);
					break;
			}
		}
		return v;
	}
};