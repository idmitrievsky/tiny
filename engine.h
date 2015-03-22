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
  std::set<std::string> _kws = { "if",  "else",  "endif", "while",  "endwhile",
                                 "var", "begin", "end",   "program" };
  std::stack<long> _stack;
  std::map<std::string, long> _vars;

  void getChar();
  void error(std::string err);
  void abort(std::string err);
  void expected(std::string s);
  void match(char c);
  std::string quote(char c);
  std::string quote(std::string s);

  bool isAlpha(char c);
  bool isDigit(char c);
  void getName();
  size_t getNum();
  void emit(std::string s);
  void emitl(std::string s);
  void program();
  void init();
  void header();
  void decl();
  void topDecls();
  bool inTable(std::string name);
  void alloc(std::string name);
  void assignment();
  void block();
  void main();
  void epilog();
  void prolog();

  void clearPm();
  void negatePm();
  void loadVal(long val);
  void undefined(std::string name);
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

  void branch(std::string l);
  void branchFalse(std::string l);
  void doIf();
  void doWhile();
  void skipws();
  void newl();

  void matchString(std::string s) {
    if (_val != s) {
      expected(s);
    }
  }

  void scan() {
    getName();
    if (_kws.find(_val) != _kws.end()) {
      _token = _val == "else" ? 'l' : _val[0];
    } else {
      _token = 'x';
    }
  }

  void lessOrEq() {
    match('=');
    exp();
    _pm = popCompare() <= 0;
  };

  void greaterOrEq() {
    match('=');
    exp();
    _pm = popCompare() >= 0;
  }
};
}

#endif /* defined(__tiny__engine__) */
