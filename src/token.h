//
//  token.h
//  tiny
//
//  Created by Иван Дмитриевский on 25/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__token__
#define __tiny__token__

#include "details.h"

#include <string>

namespace tiny {
struct token {
  enum class klass { word, num, op };
  std::string _val;
  klass _klass;
  details _info;

  bool isw() { return _klass == klass::word; }
  bool isn() { return _klass == klass::num; }
  bool iso() { return _klass == klass::op; }
};
}

#endif /* defined(__tiny__token__) */
