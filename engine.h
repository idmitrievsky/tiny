//
//  engine.h
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__engine__
#define __tiny__engine__

#include <fstream>
#include <string>

namespace tiny {
class engine {
public:
  engine(std::fstream &src);

  std::string _src;
};
}

#endif /* defined(__tiny__engine__) */
