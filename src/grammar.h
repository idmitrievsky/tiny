//
//  grammar.h
//  tiny
//
//  Created by Иван Дмитриевский on 27/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__grammar__
#define __tiny__grammar__

#include "token.h"

#include <map>
#include <vector>

namespace tiny {
class grammar {
public:
  struct lexem {
    std::string _val;
    bool _term = true;
    bool operator<(const tiny::grammar::lexem &rhs) const {
      return _val < rhs._val;
    }
  };

  grammar(std::string pathToGrammar, std::string pathToParseTable);
  std::pair<lexem, std::vector<lexem>> rule(size_t num);
  size_t predict(std::string l, token t, bool &found);
  std::vector<std::string> expected(std::string l);

  static lexem mt(std::string s);
  static lexem mnt(std::string s);

private:
  std::vector<std::pair<lexem, std::vector<lexem>>> _rules;
  std::map<std::pair<std::string, std::string>, size_t> _predicts;
};
}

#endif /* defined(__tiny__grammar__) */
