//
//  lexer.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 25/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "format.h"
#include "lexer.h"

#include <set>

namespace tiny {
std::list<token> lex::run(std::fstream &src) {
  _itr = std::istreambuf_iterator<char>(src);
  std::string ops("+-*/(<>)=,");
  std::set<std::string> keyWords = {"begin", "end", "if", "while", "print"};
  std::list<token> tokens;

  getChar();
  while (_itr != _end) {
    getWs();
    if (isAlpha(_lookAhead) || _lookAhead == '_') {
      tokens.push_back(getWord());
      if (keyWords.find(tokens.back()._val) != keyWords.end()) {
        tokens.back()._info._keyWord = true;
      }
    } else if (isDigit(_lookAhead)) {
      tokens.push_back(getNum());
    } else if (ops.find(_lookAhead) != std::string::npos) {
      tokens.push_back(getOp());
    } else {
      fmt::printf("Unknown symbol %c at %ld:%ld\n", _lookAhead, _lineNum,
                  _linePos);
      exit(EXIT_FAILURE);
    }
  }

  return tokens;
}

bool lex::isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool lex::isDigit(char c) { return '0' <= c && c <= '9'; }

bool lex::isAlphaNum(char c) { return isAlpha(c) || isDigit(c); }

bool lex::isWs(char c) { return c == ' ' || c == '\t' || c == '\n'; }

void lex::getChar() {
  _lookAhead = *_itr;
  ++_itr;
  ++_linePos;
  if (_lookAhead == '\n') {
    ++_lineNum;
    _linePos = 0;
  }
}

void lex::getWs() {
  while (isWs(_lookAhead)) {
    getChar();
  }
}

token lex::getWord() {
  token t;
  t._info._linePos = _linePos;
  t._info._lineNum = _lineNum;
  t._klass = token::klass::word;

  std::string val;
  do {
    val.push_back(_lookAhead);
    getChar();
  } while (isAlphaNum(_lookAhead) || _lookAhead == '_');

  t._val = val;
  return t;
}

token lex::getNum() {
  token t;
  t._info._linePos = _linePos;
  t._info._lineNum = _lineNum;
  t._klass = token::klass::num;

  std::string val;
  do {
    val.push_back(_lookAhead);
    getChar();
  } while (isDigit(_lookAhead));

  t._val = val;
  return t;
}

token lex::getOp() {
  token t;
  t._info._linePos = _linePos;
  t._info._lineNum = _lineNum;
  t._klass = token::klass::op;

  std::string val;
  val.push_back(_lookAhead);
  getChar();

  t._val = val;
  return t;
}
}
