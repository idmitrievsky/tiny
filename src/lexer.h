//
//  lexer.h
//  tiny
//
//  Created by Иван Дмитриевский on 25/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__lexer__
#define __tiny__lexer__

#include "token.h"

#include <iterator>
#include <fstream>
#include <list>

namespace tiny {
class lex {
public:
  std::list<token> run(std::fstream &src);

private:
  std::istreambuf_iterator<char> _itr;
  const std::istreambuf_iterator<char> _end;
  char _lookAhead = EOF;
  long _lineNum = 1;
  long _linePos = 0;

  static bool isAlpha(char c);
  static bool isDigit(char c);
  static bool isAlphaNum(char c);
  static bool isWs(char c);

  void getChar();
  void getWs();
  token getWord();
  token getNum();
  token getOp();
};
}
#endif /* defined(__tiny__lexer__) */
