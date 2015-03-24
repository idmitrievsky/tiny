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
  astexp();
  virtual long exec(context &ctx);
  void addOp(std::string op);
  void addLexp(std::shared_ptr<astexp> lexp);
  void addRexp(std::shared_ptr<astexp> rexp);
  void addVal(long val);
  void addVar(std::string varName);
  long getVal(context &ctx);

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
  astassignment();
  virtual long exec(context &ctx);
  void addVar(std::string varName);
  void addVal(std::shared_ptr<astexp> exp);

private:
  std::string _varName;
  std::shared_ptr<astexp> _exp;
};

class astprint : public ast {
public:
  astprint();
  virtual long exec(context &ctx);
  void addExp(std::shared_ptr<astexp> exp);

private:
  std::vector<std::shared_ptr<astexp> > _exps;
};

class astwhile : public ast {
public:
  astwhile();
  virtual long exec(context &ctx);
  void addCond(std::shared_ptr<astexp> cond);
  void addBody(std::shared_ptr<astblock> body);

private:
  std::shared_ptr<astexp> _cond;
  std::shared_ptr<astblock> _body;
};

class astif : public ast {
public:
  astif();
  virtual long exec(context &ctx);
  void addCond(std::shared_ptr<astexp> cond);
  void addTrue(std::shared_ptr<astblock> truth);
  void addFalse(std::shared_ptr<astblock> otherwise);

private:
  std::shared_ptr<astexp> _cond;
  std::shared_ptr<astblock> _truth;
  std::shared_ptr<astblock> _otherwise;
};

class astdecl : public ast {
public:
  astdecl(std::string varName, long val);
  virtual long exec(context &ctx);

private:
  std::string _varName;
  long _val;
};

class astblock : public ast {
public:
  astblock();
  virtual long exec(context &ctx);
  void add(std::shared_ptr<ast> astifblock);

private:
  std::vector<std::shared_ptr<ast> > _stmts;
};

class astprogram : public ast {
public:
  astprogram();
  long exec(context &ctx);
  void addDecl(astdecl decl);
  void addBlock(std::shared_ptr<astblock> block);

private:
  std::vector<astdecl> _decls;
  std::shared_ptr<astblock> _block;
};
}
#endif /* defined(__tiny__ast__) */
