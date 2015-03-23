//
//  ast.h
//  tiny
//
//  Created by Иван Дмитриевский on 23/03/15.
//  Copyright (c) 2015 Ivan Dmitrievsky. All rights reserved.
//

#ifndef __tiny__ast__
#define __tiny__ast__

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
  astexp(std::string op, std::shared_ptr<astexp> lexp,
         std::shared_ptr<astexp> rexp)
      : _op(op), _lexp(lexp), _rexp(rexp) {}
  virtual long exec(context &ctx) {
    if (_op == "+") {
      return _lexp->exec(ctx) + _lexp->_rexp(ctx);
    }
    return 0;
  }

private:
  std::string _op;
  std::shared_ptr<astexp> _lexp;
  std::shared_ptr<astexp> _rexp;
};

class astassignment : public ast {
public:
  astassignment(std::string varName, astexp exp)
      : _varName(varName), _exp(exp) {}
  virtual long exec(context &ctx) {
    long val = _exp.exec(ctx);
    ctx.setVal(_varName, val);
    return val;
  }

private:
  std::string _varName;
  astexp _exp;
};

class astwhile : public ast {
public:
  astwhile(astexp cond, std::shared_ptr<astblock> body)
      : _cond(cond), _body(body) {}
  virtual long exec(context &ctx);

private:
  astexp _cond;
  std::shared_ptr<astblock> _body;
};

class astif : public ast {
public:
  astif(astexp cond, std::shared_ptr<astblock> truth,
        std::shared_ptr<astblock> otherwise)
      : _cond(cond), _truth(truth), _otherwise(otherwise) {}
  virtual long exec(context &ctx);

private:
  astexp _cond;
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
  astprogram(astblock block) : _block(block) {}
  virtual long exec(context &ctx) {
    for (astdecl decl : _decls) {
      decl.exec(ctx);
    }
    _block.exec(ctx);
    return 0;
  }

  void addDecl(astdecl decl) { _decls.push_back(decl); }

private:
  std::vector<astdecl> _decls;
  astblock _block;
};
}
#endif /* defined(__tiny__ast__) */
