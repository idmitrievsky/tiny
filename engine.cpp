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

void engine::match(char c) {
  newl();
  if (_la == c) {
    getChar();
  } else {
    expected(quote(c));
  }
}

std::string engine::quote(char c) {
  std::string s = { '\'', c, '\'' };
  return s;
}

std::string engine::quote(std::string s) { return "'" + s + "'"; }

bool engine::isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool engine::isDigit(char c) { return '0' <= c && c <= '9'; }

char engine::getName() {
  char name;
  newl();
  if (!isAlpha(_la)) {
    expected("Name");
  }
  name = _la;
  getChar();
  return name;
}

size_t engine::getNum() {
  size_t num = 0;
  newl();
  if (!isDigit(_la)) {
    expected("Int");
  }
  while (isDigit(_la)) {
    num = 10 * num + (_la - '0');
    getChar();
  }
  return num;
}

void engine::emit(std::string s) { fmt::printf("\t%s", s); }

void engine::emitl(std::string s) { fmt::printf("\t%s\n", s); }

void engine::run() {
  init();
  program();
}

void engine::program() {
  match('p');
  header();
  topDecls();
  main();
  match('.');
}

void engine::init() {
  _idx = 0;
  getChar();
}

void engine::header() { emitl("#####"); }

void engine::decl() {
  match('v');
  alloc(getName());
  skipws();
  while (_la == ',') {
    getChar();
    alloc(getName());
    skipws();
  }
}

void engine::topDecls() {
  newl();
  while (_la != 'b') {
    if (_la == 'v') {
      decl();
    } else {
      abort("Unrecognized Keyword: " + quote(_la));
    }
    newl();
  }
}

bool engine::inTable(char name) {
  auto var = _vars.find(name);
  if (var != _vars.end()) {
    return true;
  }
  return false;
}

void engine::alloc(char name) {
  if (inTable(name)) {
    abort("Duplicate Variable Name " + quote(name));
  }

  int initVal = 1;
  if (_la == '=') {
    match('=');
    if (_la == '-') {
      match('-');
      initVal = -1;
    }
    initVal *= getNum();
  } else {
    initVal = 0;
  }
  _vars.insert({ name, initVal });
  fmt::printf("Allocate variable '%c' with %d\n", name, initVal);
}

void engine::assignment() {
  char name = getName();
  match('=');
  boolExp();
  storePm(name);
}

void engine::block() {
  newl();
  while (_la != 'e' && _la != '$') {
    switch (_la) {
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
    newl();
  }
}

void engine::main() {
  match('b');
  prolog();
  block();
  match('e');
  epilog();
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

void engine::loadVar(char name) {
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

void engine::storePm(char name) {
  if (!inTable(name)) {
    undefined(name);
  }
  emitl("STORE PM: " + quote(name) + ", " + std::to_string(_pm));
  _vars[name] = _pm;
}

void engine::undefined(char name) {
  abort("Undefined Identifier " + quote(name));
}

void engine::factor() {
  if (_la == '(') {
    match('(');
    boolExp();
    match(')');
  } else if (isAlpha(_la)) {
    loadVar(getName());
  } else {
    loadVal(getNum());
  }
}

void engine::negFactor() {
  match('-');
  if (isDigit(_la)) {
    loadVal(-getNum());
  } else {
    factor();
    negatePm();
  }
}

void engine::firstFactor() {
  switch (_la) {
    case '+':
      match('+');
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
  match('*');
  factor();
  popMul();
}

void engine::div() {
  match('/');
  factor();
  popDiv();
}

void engine::termTail() {
  newl();
  while (isMulOp(_la)) {
    pushPm();
    switch (_la) {
      case '*':
        mul();
        break;
      case '/':
        div();
        break;
      default:
        break;
    }
    newl();
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
  match('+');
  term();
  popAdd();
}

void engine::sub() {
  match('-');
  term();
  popSub();
}

void engine::exp() {
  firstTerm();
  while (isAddOp(_la)) {
    pushPm();
    switch (_la) {
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
  return op == '=' || op == '#' || op == '<' || op == '>';
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
  match('=');
  exp();
  _pm = popCompare() == 0;
}

void engine::notEqual() {
  match('#');
  exp();
  _pm = popCompare() != 0;
}

void engine::less() {
  match('<');
  exp();
  _pm = popCompare() == -1;
}

void engine::greater() {
  match('>');
  exp();
  _pm = popCompare() == 1;
}

void engine::relation() {
  exp();
  if (isRelOp(_la)) {
    pushPm();
    switch (_la) {
      case '=':
        equal();
        break;
      case '#':
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
  if (_la == '!') {
    match('!');
    relation();
    complPm();
  } else {
    relation();
  }
}

void engine::boolTerm() {
  notFactor();
  while (_la == '&') {
    pushPm();
    match('&');
    notFactor();
    popAnd();
  }
}

void engine::boolOr() {
  match('|');
  boolTerm();
  popOr();
}

void engine::boolXor() {
  match('~');
  boolTerm();
  popXor();
}

void engine::boolExp() {
  boolTerm();
  while (isOrOp(_la)) {
    pushPm();
    switch (_la) {
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
  match('i');
  boolExp();
  std::string l1 = "L0";
  std::string l2 = l1;
  branchFalse(l1);
  block();
  if (_la == '$') {
    match('$');
    l2 = "L1";
    branch(l2);
    emitl(l1);
    block();
  }
  emitl(l2);
  match('e');
}

void engine::doWhile() {
  match('w');
  std::string l1 = "L0";
  std::string l2 = "L1";
  emitl(l1);
  boolExp();
  branchFalse(l2);
  block();
  match('e');
  branch(l1);
  emitl(l2);
}

void engine::skipws() {
  while (_la == ' ') {
    getChar();
  }
}

void engine::newl() {
  skipws();
  while (_la == '\n') {
    getChar();
    skipws();
  }
}
}
