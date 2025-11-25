#ifndef BASEVISITOR_H__
#define BASEVISITOR_H__

#include <libbearpig/regexast.h>

class BaseVisitor {
public:
  virtual void visit(AlternativeExp &) = 0;
  virtual void visit(ConcatExp &) = 0;
  virtual void visit(QuantifiedExp &) = 0;
  virtual void visit(GroupExp &) = 0;
  virtual void visit(SetExp &) = 0;
  virtual void visit(SetItem &) = 0;
  virtual void visit(RChar &) = 0;
  virtual void visit(AnyExp &) = 0;
  virtual void visit(EscapeSeq &) = 0;
  virtual ~BaseVisitor() = default;
};

#endif // BASEVISITOR_H__
