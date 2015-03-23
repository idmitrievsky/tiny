//
//  ast.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "ast.h"

namespace tiny {

long astwhile::exec(context &ctx) {
  while (_cond->exec(ctx)) {
    _body->exec(ctx);
  }
  return 0;
}

long astif::exec(context &ctx) {
  if (_cond->exec(ctx)) {
    _truth->exec(ctx);
  } else {
    if (_otherwise != nullptr) {
      _otherwise->exec(ctx);
    }
  }
  return 0;
}
}
