//
//  ast.cpp
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#include "ast.h"
#include "format.h"

namespace tiny {

astexp::astexp() {}

long astexp::exec(context &ctx) {
  if (leaf) {
    return _val;
  } else if (var) {
    return ctx.getVal(_varName);
  }

  if (_op == "+") {
    return _lexp->exec(ctx) + _rexp->exec(ctx);
  } else if (_op == "-") {
    return _lexp->exec(ctx) - _rexp->exec(ctx);
  } else if (_op == "*") {
    return _lexp->exec(ctx) * _rexp->exec(ctx);
  } else if (_op == "/") {
    return _lexp->exec(ctx) / _rexp->exec(ctx);
  } else if (_op == "=") {
    return _lexp->exec(ctx) == _rexp->exec(ctx);
  } else if (_op == "$") {
    return _lexp->exec(ctx) != _rexp->exec(ctx);
  } else if (_op == ">") {
    return _lexp->exec(ctx) > _rexp->exec(ctx);
  } else if (_op == ">=") {
    return _lexp->exec(ctx) >= _rexp->exec(ctx);
  } else if (_op == "<") {
    return _lexp->exec(ctx) < _rexp->exec(ctx);
  } else if (_op == "<=") {
    return _lexp->exec(ctx) <= _rexp->exec(ctx);
  } else if (_op == "&") {
    return _lexp->exec(ctx) & _rexp->exec(ctx);
  } else if (_op == "|") {
    return _lexp->exec(ctx) | _rexp->exec(ctx);
  } else if (_op == "~") {
    return _lexp->exec(ctx) ^ _rexp->exec(ctx);
  } else if (_op == "!") {
    return ~_lexp->exec(ctx);
  } else if (_op == "@") {
    return -_lexp->exec(ctx);
  } else {
    assert(0);
  }
  return 0;
}

void astexp::addOp(std::string op) { _op = op; }

void astexp::addLexp(std::shared_ptr<astexp> lexp) { _lexp = lexp; }

void astexp::addRexp(std::shared_ptr<astexp> rexp) { _rexp = rexp; }

void astexp::addVal(long val) {
  _val = val;
  leaf = true;
}

void astexp::addVar(std::string varName) {
  _varName = varName;
  var = true;
}

long astexp::getVal(context &ctx) {
  assert(leaf || var);
  if (leaf) {
    return _val;
  } else {
    return ctx.getVal(_varName);
  }
}

astassignment::astassignment() {}

long astassignment::exec(context &ctx) {
  long val = _exp->exec(ctx);
  ctx.setVal(_varName, val);
  return val;
}

void astassignment::addVar(std::string varName) { _varName = varName; }

void astassignment::addVal(std::shared_ptr<astexp> exp) { _exp = exp; }

astprint::astprint() {}

long astprint::exec(context &ctx) {
  for (auto exp : _exps) {
    fmt::printf("%ld ", exp->exec(ctx));
  }
  fmt::printf("\n");
  return 0;
}

void astprint::addExp(std::shared_ptr<astexp> exp) { _exps.push_back(exp); }

astwhile::astwhile() {}

long astwhile::exec(context &ctx) {
  while (_cond->exec(ctx)) {
    _body->exec(ctx);
  }
  return 0;
}

void astwhile::addCond(std::shared_ptr<astexp> cond) { _cond = cond; }

void astwhile::addBody(std::shared_ptr<astblock> body) { _body = body; }

astif::astif() {}

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

void astif::addCond(std::shared_ptr<astexp> cond) { _cond = cond; }

void astif::addTrue(std::shared_ptr<astblock> truth) { _truth = truth; }

void astif::addFalse(std::shared_ptr<astblock> otherwise) {
  _otherwise = otherwise;
}

astdecl::astdecl(std::string varName, long val)
    : _varName(varName), _val(val) {}

long astdecl::exec(context &ctx) {
  ctx.regVar(_varName, _val);
  return 0;
}

astblock::astblock() {}
long astblock::exec(context &ctx) {
  for (auto stmt : _stmts) {
    stmt->exec(ctx);
  }
  return 0;
}

void astblock::add(std::shared_ptr<ast> astifblock) {
  _stmts.push_back(astifblock);
}

astprogram::astprogram() {}

long astprogram::exec(context &ctx) {
  for (astdecl decl : _decls) {
    decl.exec(ctx);
  }
  _block->exec(ctx);
  return 0;
}

void astprogram::addDecl(astdecl decl) { _decls.push_back(decl); }

void astprogram::addBlock(std::shared_ptr<astblock> block) { _block = block; }
}
