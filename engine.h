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
  std::stack<long> _stack;
  std::map<char, long> _vars;

  void getChar();
  void error(std::string err);
  void abort(std::string err);
  void expected(std::string s);
  void match(char c);
  std::string quote(char c);
  std::string quote(std::string s);

  bool isAlpha(char c);
  bool isDigit(char c);
  char getName();
  size_t getNum();
  void emit(std::string s);
  void emitl(std::string s);
  void program();
  void init();
  void header();
  void decl();
  void topDecls();
  bool inTable(char name);
  void alloc(char name);
  void assignment();
  void block();
  void main();
  void epilog();
  void prolog();

  void clearPm();
  void negatePm();
  void loadVal(long val);
  void undefined(char name);
  void loadVar(char name);
  void pushPm();
  void popAdd();
  void popSub();
  void popMul();
  void popDiv();
  void storePm(char name);

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
};
}

#endif /* defined(__tiny__engine__) */
