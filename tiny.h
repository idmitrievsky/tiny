//
//  tiny.h
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__tiny__
#define __tiny__tiny__

#include <fstream>
#include <string>

class Tiny {
public:
  Tiny(std::fstream &src);
    std::string _src;
};

#endif /* defined(__tiny__tiny__) */
