#include "lexer.h"
#include <iostream>
#include <sstream>

typedef struct Symbol {
   vector<string> input;
   string name;
};

typedef struct List {
   vector<vector<Symbol>> symbols;
};

void cSETQ(lexer a, vector<Symbol> *symbol) {
   int i = 0, j = 3;
   Symbol temp;
   for (i; i < symbol->size(); i++)
      if ((symbol->begin() + i)->name == a.getSecond(2)) {
         cout << "이미 지정된 원소입니다.";
         return;
      }
   if (i == 100) {
      cout << "원소 최대 개수를 초과했습니다.";
      return;
   }
   temp.name = a.getSecond(2);
   if (a.getFirst(j) == 10 || a.getFirst(j) == 11)
      temp.input.push_back(a.getSecond(j));
   else if (a.getFirst(j) == 13) {
      j++;
      while (a.getFirst(j) != 14) {
         temp.input.push_back(a.getSecond(j));
         j++;
      }
   }
   symbol->push_back(temp);
}

void symbolValue(lexer a, vector<Symbol> *symbol) {
   string stemp1 = a.getSecond(0), stemp2 = "";
   for (int i = 0; i < symbol->size(); i++)
      if ((symbol->begin()+i)->name == stemp1)
         for(int j=0;j<(symbol->begin()+i)->input.size();j++)
            stemp2 += (symbol->begin() + i)->input[j] + " ";
   cout << stemp2;
}

/*void makeList(lexer a) {

}*/

void check(lexer a, vector<Symbol> *symbol) {
   if (a.getFirst(0) == 13 && a.getFirst(1) == 22)
      cSETQ(a, symbol);
   else if (a.getFirst(0) == 10)
      symbolValue(a, symbol);
}