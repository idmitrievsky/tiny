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
  void error(std::string err);
  void abort(std::string err);
  void expected(std::string s);
  std::string quote(char c);
  std::string quote(std::string s);

  bool isAlpha(char c);
  bool isDigit(char c);
  void getName();
  void getNum();
  void emit(std::string s);
  void emitl(std::string s);
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

  void matchString(std::string s) {
    if (_val != s) {
      expected(s);
    }
    next();
  }

  void scan() {
    if (_token == 'x') {
      if (_kws.find(_val) != _kws.end()) {
        _token = _val == "else" ? 'l' : _val[0];
      } else {
        _token = 'x';
      }
    }
  }

  void lessOrEq() {
    next();
    exp();
    _pm = popCompare() <= 0;
  };

  void greaterOrEq() {
    next();
    exp();
    _pm = popCompare() >= 0;
  }

  void getOp() {
    skipws();
    _token = _la;
    _val.clear();
    _val.push_back(_la);
    getChar();
  }

  void next() {
    skipws();
    if (isAlpha(_la)) {
      getName();
    } else if (isDigit(_la)) {
      getNum();
    } else {
      getOp();
    }
  }

  bool isWs(char c) { return c == ' ' || c == '\t' || c == '\n'; }
};
}

#endif /* defined(__tiny__engine__) */
