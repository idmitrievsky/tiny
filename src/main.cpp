//
//  main.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "format.h"
#include <fstream>

#include "lexer.h"
#include "parser.h"

int main(int argc, const char *argv[]) {
  std::fstream input;

  if (argc > 1) {
    input.open(argv[1]);
  }

  tiny::lex l;
  tiny::parser p("/Users/ivandmi/Documents/dev/tiny/tiny/grammar.json",
                 "/Users/ivandmi/Documents/dev/tiny/tiny/table.json");

  auto lst = p.run(l.run(input));
  p.vis(lst);

  return 0;
}
