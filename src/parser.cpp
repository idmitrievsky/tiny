//
//  parser.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 27/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "parser.h"
#include "format.h"
#include <stack>

namespace tiny {
parser::parser(std::string grammarPath, std::string tablePath)
    : _gramm(grammarPath, tablePath) {}

std::list<size_t> parser::run(std::list<token> tokens) {
  std::stack<grammar::lexem> s;
  std::list<size_t> ruleNums;

  s.push(grammar::mnt("program"));
  auto token = tokens.begin();

  while (!s.empty() && token != tokens.end()) {
    auto top = s.top();

    if (!top._term) {
      bool found = false;
      size_t ruleNum = _gramm.predict(top._val, *token, found);
      if (found) {
        ruleNums.push_back(ruleNum);
        s.pop();
        auto action = _gramm.rule(ruleNum).second;
        for (auto it = action.rbegin(); it != action.rend(); ++it) {
          if (!it->_val.empty()) {
            s.push(*it);
          }
        }
      } else {
        if (token->isw() && !token->_info._keyWord && token->_val != "ident") {
          token->_val = "ident";
          continue;
        }
        if (token->isn() && token->_val != "num") {
          token->_val = "num";
          continue;
        }
        _gerror(top, *token);
        return ruleNums;
      }
    } else if (token->_val == top._val) {
      s.pop();
      ++token;
    } else if (token->isn() && top._val == "num") {
      s.pop();
      ++token;
    } else {
      _serror(top, *token);
      return ruleNums;
    }
  }

  if (!s.empty()) {
    _eoferror(s.top());
  }

  return ruleNums;
}

void parser::vis(std::list<size_t> lst) {
  for (auto num : lst) {
    auto rule = _gramm.rule(num);
    fmt::printf("<%s> -> ", rule.first._val);
    auto rem = rule.second;

    if (rem[0]._term && !rem[0]._val.empty()) {
      fmt::printf("%s", rem[0]._val);
    } else {
      fmt::printf("<%s>", rem[0]._val);
    }

    for (auto it = ++rem.begin(); it != rem.end(); ++it) {
      if (it->_term && !it->_val.empty()) {
        fmt::printf(", %s", it->_val);
      } else {
        fmt::printf(", <%s>", it->_val);
      }
    }
    fmt::printf("\n");
  }
}

void parser::_gerror(grammar::lexem l, token t) {
  auto expected = _gramm.expected(l._val);
  std::string err =
      fmt::sprintf("Unexpected word %s at %ld:%ld. Expected %s", t._val,
                   t._info._lineNum, t._info._linePos, expected[0]);
  for (auto it = expected.begin() + 1; it != expected.end(); ++it) {
    err += ", " + *it;
  }
  err += ".\n";
  fmt::printf("%s", err);
}

void parser::_eoferror(grammar::lexem l) {
  auto expected = _gramm.expected(l._val);
  std::string err =
      fmt::sprintf("Unexpected end of file. Expected %s", expected[0]);
  for (auto it = expected.begin() + 1; it != expected.end(); ++it) {
    err += ", " + *it;
  }
  err += ".\n";
  fmt::printf("%s", err);
}

void parser::_serror(grammar::lexem l, token t) {
  fmt::printf("Unexpected word %s at %ld:%ld. Expected %s.\n", t._val,
              t._info._lineNum, t._info._linePos, l._val);
}
}
