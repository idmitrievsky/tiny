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

#include "ast.h"
#include "context.h"

namespace tiny {
class engine {
public:
  engine(std::fstream &src);
  void run();

private:
  size_t _idx;
  char _la;
  std::shared_ptr<astexp> _pm = std::make_shared<astexp>();
  std::string _src;
  std::string _val;
  char _token;
  std::set<std::string> _kws = { "if", "else", "while", "print", "var", "end" };
  std::stack<std::shared_ptr<astexp> > _stack;
  context _ctx;
  astprogram _ast;

  void getChar();
  void getName();
  void getNum();
  void init();
  void header();
  void decl();
  void alloc();
  std::shared_ptr<ast> assignment();
  std::shared_ptr<astblock> block();
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
  void equal();
  void notEqual();
  void less();
  void greater();
  void lessOrEq();
  void greaterOrEq();
  void relation();
  void notFactor();
  void boolTerm();
  void boolOr();
  void boolXor();
  std::shared_ptr<astexp> boolExp();

  std::shared_ptr<ast> doIf();
  std::shared_ptr<ast> doWhile();
  std::shared_ptr<ast> doPrint();
  void skipws();

  void matchString(std::string s);
  void scan();
  void getOp();
  void next();
};
}

#endif /* defined(__tiny__engine__) */
