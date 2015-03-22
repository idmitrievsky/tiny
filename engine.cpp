//
//  engine.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 22/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "engine.h"

#include <streambuf>

namespace tiny {
engine::engine(std::fstream &src)
    : _src(std::string(std::istreambuf_iterator<char>(src),
                       std::istreambuf_iterator<char>())) {}
}