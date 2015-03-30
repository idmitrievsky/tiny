//
//  engine.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "engine.h"
#include "utils.h"

#include <streambuf>

namespace tiny {
engine::engine(std::fstream &src)
    : _src(std::string(std::istreambuf_iterator<char>(src),
                       std::istreambuf_iterator<char>())),
      _la(_src[0]), _idx(1) {}

void engine::getChar() { _la = _src[_idx++]; }

void engine::getName() {
  skipws();
  if (!utils::isAlpha(_la)) {
    utils::expected("Identifier", false);
  }
  _val.clear();
  _token = 'x';
  do {
    _val.push_back(_la);
    getChar();
  } while (utils::isAlpha(_la));
}

void engine::getNum() {
  skipws();
  if (!utils::isDigit(_la)) {
    utils::expected("Number", false);
  }
  _token = '#';
  _val.clear();
  do {
    _val.push_back(_la);
    getChar();
  } while (utils::isDigit(_la));
}

void engine::run() {
  init();
  matchString("program");
  header();
  decl();
  matchString("begin");
  prolog();
  _ast.addBlock(block());
  matchString("end");
  context ctx;
  _ast.exec(ctx);
  epilog();
}

void engine::init() {
  _idx = 0;
  getChar();
  next();
}

void engine::header() { utils::emitl("#####"); }

void engine::decl() {
  scan();
  while (_token == 'v') {
    alloc();
    while (_token == ',') {
      alloc();
    }
    scan();
  }
}

void engine::alloc() {
  next();
  scan();
  if (_token != 'x') {
    utils::expected("Variable", false);
  }

  std::string name = _val;
  int initVal = 1;
  next();
  if (_token == '=') {
    matchString("=");
    if (_token == '-') {
      matchString("-");
      initVal = -1;
    }
    if (_token == '#') {
      initVal *= std::stol(_val);
    } else if (_token == 'x') {
      initVal *= _ctx.getVal(_val);
    } else {
      utils::expected("Value", false);
    }
    next();
  } else {
    initVal = 0;
  }
  _ctx.regVar(name, initVal);
  _ast.addDecl(astdecl(name, initVal));
  utils::allocated(name, initVal);
}

std::shared_ptr<ast> engine::assignment() {
  auto assignment = std::make_shared<astassignment>();
  std::string name = _val;
  assignment->addVar(name);
  next();
  matchString("=");
  assignment->addVal(boolExp());
  return assignment;
}

std::shared_ptr<astblock> engine::block() {
  scan();
  auto block = std::make_shared<astblock>();
  while (_token != 'e' && _token != 'l') {
    switch (_token) {
    case 'i':
      block->add(doIf());
      break;
    case 'w':
      block->add(doWhile());
      break;
    case 'p':
      block->add(doPrint());
      break;
    default:
      block->add(assignment());
      break;
    }
    scan();
  }
  return block;
}

void engine::prolog() { utils::emitl("BEGIN MAIN"); }

void engine::epilog() { utils::emitl("END MAIN"); }

void engine::clearPm() {
  utils::emitl("CLEAR PM");
  _pm = std::make_shared<astexp>();
}

void engine::loadVal(long val) {
  utils::emitl("LOAD VAL PM: " + std::to_string(val));
  _pm->addVal(val);
}

void engine::loadVar(std::string name) {
  _pm->addVar(name);
  utils::loadedVar(name, _pm->getVal(_ctx));
}

void engine::pushPm() {
  utils::emitl("PUSH PM");
  _stack.push(_pm);
  clearPm();
}

void engine::popAdd() {
  utils::emitl("POP ADD");
  auto op = std::make_shared<astexp>();
  op->addOp("+");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::popSub() {
  utils::emitl("POP SUB");
  auto op = std::make_shared<astexp>();
  op->addOp("-");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::popMul() {
  utils::emitl("POP MUL");
  auto op = std::make_shared<astexp>();
  op->addOp("*");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::popDiv() {
  utils::emitl("POP DIV");
  auto op = std::make_shared<astexp>();
  op->addOp("/");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::storePm(std::string name) {
  _ctx.setVal(name, _pm->getVal(_ctx));
  utils::storedVar(name, _pm->getVal(_ctx));
}

void engine::factor() {
  if (_token == '(') {
    matchString("(");
    _pm = boolExp();
    matchString(")");
  } else {
    if (_token == 'x') {
      loadVar(_val);
    } else if (_token == '#') {
      loadVal(std::stol(_val));
    } else {
      utils::expected("Factor", false);
    }
    next();
  }
}

void engine::negFactor() {
  next();
  if (_token == '#') {
    loadVal(-std::stol(_val));
    next();
  } else {
    factor();
    negatePm();
  }
}

void engine::firstFactor() {
  switch (_token) {
  case '+':
    next();
    factor();
    break;
  case '-':
    negFactor();
    break;
  default:
    factor();
    break;
  }
}

void engine::mul() {
  next();
  factor();
  popMul();
}

void engine::div() {
  next();
  factor();
  popDiv();
}

void engine::termTail() {
  while (isMulOp(_token)) {
    pushPm();
    switch (_token) {
    case '*':
      mul();
      break;
    case '/':
      div();
      break;
    default:
      break;
    }
  }
}

void engine::term() {
  factor();
  termTail();
}

void engine::firstTerm() {
  firstFactor();
  termTail();
}

void engine::add() {
  next();
  term();
  popAdd();
}

void engine::sub() {
  next();
  term();
  popSub();
}

void engine::exp() {
  firstTerm();
  while (isAddOp(_token)) {
    pushPm();
    switch (_token) {
    case '+':
      add();
      break;
    case '-':
      sub();
    default:
      break;
    }
  }
}

bool engine::isAddOp(char op) { return op == '+' || op == '-'; }

bool engine::isMulOp(char op) { return op == '*' || op == '/'; }

bool engine::isOrOp(char op) { return op == '|' || op == '~'; }

bool engine::isRelOp(char op) {
  return op == '=' || op == '$' || op == '<' || op == '>';
}

void engine::negatePm() {
  utils::emitl("COMPLEMENT PM");
  auto op = std::make_shared<astexp>();
  op->addOp("@");
  op->addLexp(_pm);
  _pm = op;
}

void engine::complPm() {
  utils::emitl("COMPLEMENT PM");
  auto op = std::make_shared<astexp>();
  op->addOp("!");
  op->addLexp(_pm);
  _pm = op;
}

void engine::popAnd() {
  utils::emitl("POP AND");
  auto op = std::make_shared<astexp>();
  op->addOp("&");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::popOr() {
  utils::emitl("POP OR");
  auto op = std::make_shared<astexp>();
  op->addOp("|");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::popXor() {
  utils::emitl("POP XOR");
  auto op = std::make_shared<astexp>();
  op->addOp("^");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::equal() {
  next();
  exp();
  auto op = std::make_shared<astexp>();
  op->addOp("=");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::notEqual() {
  next();
  exp();
  auto op = std::make_shared<astexp>();
  op->addOp("$");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::less() {
  next();
  switch (_token) {
  case '=':
    lessOrEq();
    break;
  default:
    exp();
    auto op = std::make_shared<astexp>();
    op->addOp("<");
    op->addLexp(_stack.top());
    op->addRexp(_pm);
    _stack.pop();
    _pm = op;
    break;
  }
}

void engine::greater() {
  next();
  switch (_token) {
  case '=':
    greaterOrEq();
    break;
  default:
    exp();
    auto op = std::make_shared<astexp>();
    op->addOp(">");
    op->addLexp(_stack.top());
    op->addRexp(_pm);
    _stack.pop();
    _pm = op;
    break;
  }
}

void engine::relation() {
  exp();
  if (isRelOp(_token)) {
    pushPm();
    switch (_token) {
    case '=':
      equal();
      break;
    case '$':
      notEqual();
      break;
    case '<':
      less();
      break;
    case '>':
      greater();
      break;
    default:
      break;
    }
  }
}

void engine::notFactor() {
  if (_token == '!') {
    next();
    relation();
    complPm();
  } else {
    relation();
  }
}

void engine::boolTerm() {
  notFactor();
  while (_token == '&') {
    pushPm();
    next();
    notFactor();
    popAnd();
  }
}

void engine::boolOr() {
  next();
  boolTerm();
  popOr();
}

void engine::boolXor() {
  next();
  boolTerm();
  popXor();
}

std::shared_ptr<astexp> engine::boolExp() {
  boolTerm();
  while (isOrOp(_token)) {
    pushPm();
    switch (_token) {
    case '|':
      boolOr();
      break;
    case '~':
      boolXor();
      break;
    default:
      break;
    }
  }
  std::shared_ptr<astexp> tmp = _pm;
  clearPm();
  return tmp;
}

std::shared_ptr<ast> engine::doIf() {
  next();
  auto ifblock = std::make_shared<astif>();
  ifblock->addCond(boolExp());
  std::string l1 = "L0";
  std::string l2 = l1;
  utils::branchFalse(l1);
  ifblock->addTrue(block());
  if (_token == 'l') {
    next();
    l2 = "L1";
    utils::branch(l2);
    utils::emitl(l1);
    ifblock->addFalse(block());
  }
  utils::emitl(l2);
  matchString("end");
  return ifblock;
}

std::shared_ptr<ast> engine::doWhile() {
  next();
  auto whileblock = std::make_shared<astwhile>();
  std::string l1 = "L0";
  std::string l2 = "L1";
  utils::emitl(l1);
  whileblock->addCond(boolExp());
  utils::branchFalse(l2);
  whileblock->addBody(block());
  matchString("end");
  utils::branch(l1);
  utils::emitl(l2);
  return whileblock;
}

std::shared_ptr<ast> engine::doPrint() {
  next();
  auto printblock = std::make_shared<astprint>();
  //  matchString("(");
  printblock->addExp(boolExp());
  while (_token == ',') {
    next();
    printblock->addExp(boolExp());
  }
  //  matchString(")");
  return printblock;
}

void engine::skipws() {
  while (utils::isWs(_la)) {
    getChar();
  }
}

void engine::matchString(std::string s) {
  if (_val != s) {
    utils::expected(s);
  }
  next();
}

void engine::scan() {
  if (_token == 'x') {
    if (_kws.find(_val) != _kws.end()) {
      _token = _val == "else" ? 'l' : _val[0];
    } else {
      _token = 'x';
    }
  }
}

void engine::lessOrEq() {
  next();
  exp();
  auto op = std::make_shared<astexp>();
  op->addOp("<=");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
};

void engine::greaterOrEq() {
  next();
  exp();
  auto op = std::make_shared<astexp>();
  op->addOp(">=");
  op->addLexp(_stack.top());
  op->addRexp(_pm);
  _stack.pop();
  _pm = op;
}

void engine::getOp() {
  skipws();
  _token = _la;
  _val.clear();
  _val.push_back(_la);
  getChar();
}

void engine::next() {
  skipws();
  if (utils::utils::isAlpha(_la)) {
    getName();
  } else if (utils::utils::isDigit(_la)) {
    getNum();
  } else {
    getOp();
  }
}
}