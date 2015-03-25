//
//  utils.h
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__utils__
#define __tiny__utils__

#include <string>

namespace tiny {
class utils {
public:
  static void error(std::string err);
  static void abort(std::string err);
  static void abortDuplicate(std::string var);
  static void expected(std::string s, bool quote = true);
  static void runtime(std::string s);
  static void undefined(std::string var);

  static void emit(std::string s);
  static void emitl(std::string s);
  static void allocated(std::string var, long val);
  static void loadedVar(std::string var, long val);
  static void storedVar(std::string var, long val);
  static void branch(std::string l);
  static void branchFalse(std::string l);

  static bool isAlpha(char c);
  static bool isDigit(char c);
  static bool isWs(char c);

private:
  static std::string quote(char c);
  static std::string quote(std::string s);
};
}

#endif /* defined(__tiny__utils__) */
