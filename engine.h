//
//  engine.h
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__engine__
#define __tiny__engine__

#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <string>

namespace tiny {
class engine {
public:
  engine(std::fstream &src);
  void run();

private:
  size_t _idx;
  char _la;
  long _pm;
  std::string _src;
  std::string _val;
  char _token;
  std::set<std::string> _kws = { "if",       "else", "endif", "while",
                                 "endwhile", "var",  "end" };
  std::stack<long> _stack;
  std::map<std::string, long> _vars;

  void getChar();

  void getName();
  void getNum();
  void init();
  void header();
  void topDecls();
  bool inTable(std::string name);
  void alloc();
  void assignment();
  void block();
  void epilog();
  void prolog();

  void clearPm();
  void negatePm();
  void loadVal(long val);
  void loadVar(std::string name);
  void pushPm();
  void popAdd();
  void popSub();
  void popMul();
  void popDiv();
  void storePm(std::string name);

  bool isAddOp(char op);
  bool isMulOp(char op);

  void factor();
  void negFactor();
  void firstFactor();
  void mul();
  void div();
  void termTail();
  void term();
  void firstTerm();
  void add();
  void sub();
  void exp();

  bool isOrOp(char op);
  bool isRelOp(char op);
  void complPm();
  void popAnd();
  void popOr();
  void popXor();
  short popCompare();
  void equal();
  void notEqual();
  void less();
  void greater();
  void relation();
  void notFactor();
  void boolTerm();
  void boolOr();
  void boolXor();
  void boolExp();

  void doIf();
  void doWhile();
  void skipws();

  void matchString(std::string s);
  void scan();

  void lessOrEq();

  void greaterOrEq();
  void getOp();
  void next();
};
}

#endif /* defined(__tiny__engine__) */
