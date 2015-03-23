//
//  utils.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "utils.h"

#include "format.h"

namespace tiny {
void utils::error(std::string err) { fmt::printf("%s\n", err); }

void utils::abort(std::string err) {
  fmt::printf("%s\n", err);
  exit(EXIT_FAILURE);
}

void utils::abortDuplicate(std::string var) {
  abort("Duplicate Variable Name " + quote(var));
}

void utils::expected(std::string s, bool quot) {
  if (quot) {
    s = quote(s);
  }
  abort("Expected: " + s + "\n");
}

void utils::undefined(std::string var) {
  abort("Undefined Identifier " + quote(var));
}

void utils::emit(std::string s) { fmt::printf("\t%s", s); }

void utils::emitl(std::string s) { fmt::printf("\t%s\n", s); }

void utils::allocated(std::string var, long val) {
  fmt::printf("Allocate variable '%s' with %d\n", var, val);
}

void utils::loadedVar(std::string var, long val) {
  emitl("LOAD VAR PM: " + quote(var) + ", " + std::to_string(val));
}

void utils::storedVar(std::string var, long val) {
  emitl("STORE PM: " + quote(var) + ", " + std::to_string(val));
}

void utils::branch(std::string l) { emitl("BRA " + l); }

void utils::branchFalse(std::string l) {
  emitl("TST D0");
  emitl("BEQ" + l);
}

bool utils::isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool utils::isDigit(char c) { return '0' <= c && c <= '9'; }

bool utils::isWs(char c) { return c == ' ' || c == '\t' || c == '\n'; }

std::string utils::quote(char c) {
  std::string s = { '\'', c, '\'' };
  return s;
}

std::string utils::quote(std::string s) { return "'" + s + "'"; }
}
