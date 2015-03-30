//
//  grammar.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 27/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "grammar.h"

#include "json11.h"

#include <fstream>
#include <streambuf>

namespace tiny {
grammar::grammar(std::string pathToGrammar, std::string pathToParseTable) {
  std::fstream stream;
  stream.open(pathToGrammar);

  std::string err, doc((std::istreambuf_iterator<char>(stream)),
                       std::istreambuf_iterator<char>());
  json11::Json grammDesc = json11::Json::parse(doc, err);

  if (grammDesc.is_array()) {
    for (auto rule : grammDesc.array_items()) {
      if (rule.is_object()) {
        for (auto obj : rule.object_items()) {
          _rules.push_back({mnt(obj.first), {}});
          if (obj.second.is_array()) {
            for (auto word : obj.second.array_items()) {
              if (word.is_string()) {
                grammar::lexem grammLexem;
                std::string str = word.string_value();
                if (str[0] == '`') {
                  grammLexem = mt(str.substr(1));
                } else {
                  grammLexem = mnt(str);
                }
                _rules[_rules.size() - 1].second.push_back(grammLexem);
              }
            }
          }
        }
      }
    }
  }

  stream.close();
  stream.open(pathToParseTable);
  doc = std::string((std::istreambuf_iterator<char>(stream)),
                    std::istreambuf_iterator<char>());
  json11::Json tableDesc = json11::Json::parse(doc, err);

  if (tableDesc.is_object()) {
    for (auto pairs : tableDesc.object_items()) {
      if (pairs.second.is_array()) {
        for (auto pair : pairs.second.array_items()) {
          for (auto ruleMap : pair.object_items()) {
            _predicts.insert(
                {{pairs.first, ruleMap.first}, ruleMap.second.int_value() - 1});
          }
        }
      }
    }
  }
}

std::pair<grammar::lexem, std::vector<grammar::lexem>>
grammar::rule(size_t num) {
  return _rules[num];
}

size_t grammar::predict(std::string l, token t, bool &found) {
  auto num = _predicts.find({l, t._val});
  found = (num != _predicts.end());
  return num->second;
}

std::vector<std::string> grammar::expected(std::string l) {
  std::vector<std::string> exp;
  for (auto p : _predicts) {
    if (p.first.first == l) {
      exp.push_back(p.first.second);
    }
  }
  return exp;
}

grammar::lexem grammar::mt(std::string s) {
  grammar::lexem l;
  l._val = s;
  l._term = true;
  return l;
}

grammar::lexem grammar::mnt(std::string s) {
  grammar::lexem l;
  l._val = s;
  l._term = false;
  return l;
}
}
