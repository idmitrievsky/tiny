//
//  parser.h
//  tiny
//
//  Created by Иван Дмитриевский on 27/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__parser__
#define __tiny__parser__

#include <list>
#include <string>
#include "token.h"
#include "grammar.h"

namespace tiny {
class parser {
public:
  parser(std::string grammarPath, std::string tablePath);
  std::list<size_t> run(std::list<token>);
  void vis(std::list<size_t>);

private:
  grammar _gramm;
  void _gerror(grammar::lexem l, token t);
  void _eoferror(grammar::lexem l);
  void _serror(grammar::lexem l, token t);
};
}

#endif /* defined(__tiny__parser__) */
