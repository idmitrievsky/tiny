//
//  context.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "context.h"
#include "utils.h"

namespace tiny {
bool context::inTable(std::string varName) {
  auto var = _vars.find(varName);
  if (var != _vars.end()) {
    return true;
  }
  return false;
}

void context::regVar(std::string varName, long val) {
  if (inTable(varName)) {
    utils::abortDuplicate(varName);
  }
  _vars.insert({ varName, val });
}

long context::getVal(std::string varName) {
  if (!inTable(varName)) {
    utils::undefined(varName);
  }
  return _vars[varName];
}

void context::setVal(std::string varName, long val) {
  if (!inTable(varName)) {
    utils::undefined(varName);
  }
  _vars[varName] = val;
}
}
