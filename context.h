//
//  context.h
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__context__
#define __tiny__context__

#include <map>
#include <string>

namespace tiny {
class context {
public:
  bool inTable(std::string varName);
  void regVar(std::string varName, long val);
  long getVal(std::string varName);
  void setVal(std::string varName, long val);

private:
  std::map<std::string, long> _vars;
};
}

#endif /* defined(__tiny__context__) */
