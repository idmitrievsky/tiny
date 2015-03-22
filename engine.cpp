//
//  engine.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "engine.h"
#include "format.h"

#include <streambuf>

namespace tiny {
engine::engine(std::fstream &src)
    : _src(std::string(std::istreambuf_iterator<char>(src),
                       std::istreambuf_iterator<char>())),
      _la(_src[0]),
      _idx(1) {}

void engine::getChar() { _la = _src[_idx++]; }

void engine::error(std::string err) { fmt::printf("%s\n", err); }

void engine::abort(std::string err) {
  fmt::printf("%s\n", err);
  exit(EXIT_FAILURE);
}

void engine::expected(std::string s) { abort("Expected: " + s + "\n"); }

std::string engine::quote(char c) {
  std::string s = { '\'', c, '\'' };
  return s;
}

std::string engine::quote(std::string s) { return "'" + s + "'"; }

bool engine::isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool engine::isDigit(char c) { return '0' <= c && c <= '9'; }

void engine::getName() {
  skipws();
  if (!isAlpha(_la)) {
    expected("Identifier");
  }
  _val.clear();
  _token = 'x';
  do {
    _val.push_back(_la);
    getChar();
  } while (isAlpha(_la));
}

void engine::getNum() {
  skipws();
  if (!isDigit(_la)) {
    expected("Number");
  }
  _token = '#';
  _val.clear();
  do {
    _val.push_back(_la);
    getChar();
  } while (isDigit(_la));
}

void engine::emit(std::string s) { fmt::printf("\t%s", s); }

void engine::emitl(std::string s) { fmt::printf("\t%s\n", s); }

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

void engine::header() { emitl("#####"); }

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
    expected("Variable");
  }

  if (inTable(_val)) {
    abort("Duplicate Variable Name " + quote(_val));
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
      expected("Variable");
    }
    initVal *= std::stol(_val);
  } else {
    initVal = 0;
  }
  _vars.insert({ name, initVal });
  fmt::printf("Allocate variable '%s' with %d\n", name, initVal);
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

void engine::prolog() { emitl("BEGIN MAIN"); }

void engine::epilog() { emitl("END MAIN"); }

void engine::clearPm() {
  emitl("CLEAR PM");
  _pm = 0;
}
void engine::negatePm() {
  emitl("NEGATE PM");
  _pm *= -1;
}

void engine::loadVal(long val) {
  emitl("LOAD VAL PM: " + std::to_string(val));
  _pm = val;
}

void engine::loadVar(std::string name) {
  if (!inTable(name)) {
    undefined(name);
  }
  emitl("LOAD VAR PM: " + quote(name));
  _pm = _vars[name];
}

void engine::pushPm() {
  emitl("PUSH PM");
  _stack.push(_pm);
}

void engine::popAdd() {
  emitl("POP ADD");
  _pm += _stack.top();
  _stack.pop();
}

void engine::popSub() {
  emitl("POP SUB");
  _pm = _stack.top() - _pm;
  _stack.pop();
}

void engine::popMul() {
  emitl("POP MUL");
  _pm *= _stack.top();
  _stack.pop();
}

void engine::popDiv() {
  emitl("POP DIV");
  _pm = _stack.top() / _pm;
  _stack.pop();
}

void engine::storePm(std::string name) {
  if (!inTable(name)) {
    undefined(name);
  }
  emitl("STORE PM: " + quote(name) + ", " + std::to_string(_pm));
  _vars[name] = _pm;
}

void engine::undefined(std::string name) {
  abort("Undefined Identifier " + quote(name));
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
      expected("Factor");
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
  emitl("COMPLEMENT PM");
  _pm = ~_pm;
}

void engine::popAnd() {
  emitl("POP AND");
  _pm &= _stack.top();
  _stack.pop();
}

void engine::popOr() {
  emitl("POP OR");
  _pm |= _stack.top();
  _stack.pop();
}

void engine::popXor() {
  emitl("POP XOR");
  _pm ^= _stack.top();
  _stack.pop();
}

short engine::popCompare() {
  emitl("POP CMP");
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

void engine::branch(std::string l) { emitl("BRA" + l); }

void engine::branchFalse(std::string l) {
  emitl("TST D0");
  emitl("BEQ" + l);
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
    emitl(l1);
    block();
  }
  emitl(l2);
  matchString("endif");
}

void engine::doWhile() {
  next();
  std::string l1 = "L0";
  std::string l2 = "L1";
  emitl(l1);
  boolExp();
  branchFalse(l2);
  block();
  matchString("endwhile");
  branch(l1);
  emitl(l2);
}

void engine::skipws() {
  while (isWs(_la)) {
    getChar();
  }
}
}
