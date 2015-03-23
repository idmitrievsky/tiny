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
      _la(_src[0]),
      _idx(1) {}

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
  topDecls();
  next();
  matchString("begin");
  prolog();
  block();
  matchString("end");
  epilog();
}

void engine::init() {
  _idx = 0;
  getChar();
  next();
}

void engine::header() { utils::emitl("#####"); }

void engine::topDecls() {
  scan();
  while (_token == 'v') {
    alloc();
    next();
    while (_token == ',') {
      alloc();
    }
    scan();
  }
}

bool engine::inTable(std::string name) {
  auto var = _vars.find(name);
  if (var != _vars.end()) {
    return true;
  }
  return false;
}

void engine::alloc() {
  next();
  if (_token != 'x') {
    utils::expected("Variable", false);
  }

  if (inTable(_val)) {
    utils::abortDuplicate(_val);
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
    if (_token != '#') {
      utils::expected("Variable", false);
    }
    initVal *= std::stol(_val);
  } else {
    initVal = 0;
  }
  _vars.insert({ name, initVal });
}

void engine::assignment() {
  std::string name = _val;
  next();
  matchString("=");
  boolExp();
  storePm(name);
}

void engine::block() {
  scan();
  while (_token != 'e' && _token != 'l') {
    switch (_token) {
      case 'i':
        doIf();
        break;
      case 'w':
        doWhile();
        break;
      default:
        assignment();
        break;
    }
    scan();
  }
}

void engine::prolog() { utils::emitl("BEGIN MAIN"); }

void engine::epilog() { utils::emitl("END MAIN"); }

void engine::clearPm() {
  utils::emitl("CLEAR PM");
  _pm = 0;
}
void engine::negatePm() {
  utils::emitl("NEGATE PM");
  _pm *= -1;
}

void engine::loadVal(long val) {
  utils::emitl("LOAD VAL PM: " + std::to_string(val));
  _pm = val;
}

void engine::loadVar(std::string name) {
  if (!inTable(name)) {
    utils::undefined(name);
  }
  utils::loadedVar(name, _pm);
  _pm = _vars[name];
}

void engine::pushPm() {
  utils::emitl("PUSH PM");
  _stack.push(_pm);
}

void engine::popAdd() {
  utils::emitl("POP ADD");
  _pm += _stack.top();
  _stack.pop();
}

void engine::popSub() {
  utils::emitl("POP SUB");
  _pm = _stack.top() - _pm;
  _stack.pop();
}

void engine::popMul() {
  utils::emitl("POP MUL");
  _pm *= _stack.top();
  _stack.pop();
}

void engine::popDiv() {
  utils::emitl("POP DIV");
  _pm = _stack.top() / _pm;
  _stack.pop();
}

void engine::storePm(std::string name) {
  if (!inTable(name)) {
    utils::undefined(name);
  }
  utils::storedVar(name, _pm);
  _vars[name] = _pm;
}

void engine::factor() {
  if (_token == '(') {
    matchString("(");
    boolExp();
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

void engine::complPm() {
  utils::emitl("COMPLEMENT PM");
  _pm = ~_pm;
}

void engine::popAnd() {
  utils::emitl("POP AND");
  _pm &= _stack.top();
  _stack.pop();
}

void engine::popOr() {
  utils::emitl("POP OR");
  _pm |= _stack.top();
  _stack.pop();
}

void engine::popXor() {
  utils::emitl("POP XOR");
  _pm ^= _stack.top();
  _stack.pop();
}

short engine::popCompare() {
  utils::emitl("POP CMP");
  short rel = 0;

  if (_pm == _stack.top()) {
    rel = 0;
  } else if (_stack.top() < _pm) {
    rel = -1;
  } else {
    rel = 1;
  }

  _stack.pop();

  return rel;
}

void engine::equal() {
  next();
  exp();
  _pm = popCompare() == 0;
}

void engine::notEqual() {
  next();
  exp();
  _pm = popCompare() != 0;
}

void engine::less() {
  next();
  switch (_token) {
    case '=':
      lessOrEq();
      break;
    default:
      exp();
      _pm = popCompare() == -1;
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
      _pm = popCompare() == 1;
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

void engine::boolExp() {
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
}

void engine::branch(std::string l) { utils::emitl("BRA" + l); }

void engine::branchFalse(std::string l) {
  utils::emitl("TST D0");
  utils::emitl("BEQ" + l);
}

void engine::doIf() {
  next();
  boolExp();
  std::string l1 = "L0";
  std::string l2 = l1;
  branchFalse(l1);
  block();
  if (_token == 'l') {
    next();
    l2 = "L1";
    branch(l2);
    utils::emitl(l1);
    block();
  }
  utils::emitl(l2);
  matchString("endif");
}

void engine::doWhile() {
  next();
  std::string l1 = "L0";
  std::string l2 = "L1";
  utils::emitl(l1);
  boolExp();
  branchFalse(l2);
  block();
  matchString("endwhile");
  branch(l1);
  utils::emitl(l2);
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
  _pm = popCompare() <= 0;
};

void engine::greaterOrEq() {
  next();
  exp();
  _pm = popCompare() >= 0;
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
