//
//  ast.h
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__ast__
#define __tiny__ast__

#include <assert.h>
#include <string>
#include <memory>
#include <vector>

#include "context.h"

namespace tiny {
class ast {
public:
  virtual long exec(context &ctx) = 0;
  virtual ~ast(){};
};

class astblock;

class astexp : public ast {
public:
  astexp() {}
  virtual long exec(context &ctx) {
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
  void addOp(std::string op) { _op = op; }
  void addLexp(std::shared_ptr<astexp> lexp) { _lexp = lexp; }
  void addRexp(std::shared_ptr<astexp> rexp) { _rexp = rexp; }
  void addVal(long val) {
    _val = val;
    leaf = true;
  }
  void addVar(std::string varName) {
    _varName = varName;
    var = true;
  }
  long getVal(context &ctx) {
    assert(leaf || var);
    if (leaf) {
      return _val;
    } else {
      return ctx.getVal(_varName);
    }
  }

private:
  std::string _op;
  std::shared_ptr<astexp> _lexp;
  std::shared_ptr<astexp> _rexp;
  long _val;
  std::string _varName;
  bool leaf = false, var = false;
};

class astassignment : public ast {
public:
  astassignment() {}
  virtual long exec(context &ctx) {
    long val = _exp->exec(ctx);
    ctx.setVal(_varName, val);
    return val;
  }

  void addVar(std::string varName) { _varName = varName; }
  void addVal(std::shared_ptr<astexp> exp) { _exp = exp; }

private:
  std::string _varName;
  std::shared_ptr<astexp> _exp;
};

class astwhile : public ast {
public:
  astwhile() {}
  virtual long exec(context &ctx);

  void addCond(std::shared_ptr<astexp> cond) { _cond = cond; }

  void addBody(std::shared_ptr<astblock> body) { _body = body; }

private:
  std::shared_ptr<astexp> _cond;
  std::shared_ptr<astblock> _body;
};

class astif : public ast {
public:
  astif() {}
  virtual long exec(context &ctx);

  void addCond(std::shared_ptr<astexp> cond) { _cond = cond; }

  void addTrue(std::shared_ptr<astblock> truth) { _truth = truth; }

  void addFalse(std::shared_ptr<astblock> otherwise) { _otherwise = otherwise; }

private:
  std::shared_ptr<astexp> _cond;
  std::shared_ptr<astblock> _truth;
  std::shared_ptr<astblock> _otherwise;
};

class astdecl : public ast {
public:
  astdecl(std::string varName, long val = 0) : _varName(varName), _val(val) {}
  virtual long exec(context &ctx) {
    ctx.regVar(_varName, _val);
    return 0;
  }

private:
  std::string _varName;
  long _val;
};

class astblock : public ast {
public:
  astblock() {}
  virtual long exec(context &ctx) {
    for (auto stmt : _stmts) {
      stmt->exec(ctx);
    }
    return 0;
  }

  void add(std::shared_ptr<ast> astifblock) { _stmts.push_back(astifblock); }

private:
  std::vector<std::shared_ptr<ast> > _stmts;
};

class astprogram : public ast {
public:
  astprogram() {}
  virtual long exec(context &ctx) {
    for (astdecl decl : _decls) {
      decl.exec(ctx);
    }
    _block->exec(ctx);
    return 0;
  }

  void addDecl(astdecl decl) { _decls.push_back(decl); }
  void addBlock(std::shared_ptr<astblock> block) { _block = block; }

private:
  std::vector<astdecl> _decls;
  std::shared_ptr<astblock> _block;
};
}
#endif /* defined(__tiny__ast__) */
