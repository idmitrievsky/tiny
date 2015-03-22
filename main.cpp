//
//  main.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "engine.h"

int main(int argc, const char* argv[]) {
  std::fstream input;

  if (argc > 1) {
    input.open(argv[1]);
  }

  tiny::engine e(input);
  std::cout << e._src;
  return 0;
}
