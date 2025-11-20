#ifndef PRINTVISITOR_H__
#define PRINTVISITOR_H__

#include <libbearpig/basevisitor.h>

class PrintVisitor : public BaseVisitor {
public:
  void visit(AlternativeExp &) override;
  void visit(ConcatExp &) override;
  void visit(QuantifiedExp &) override;
  void visit(GroupExp &) override;
  void visit(SetExp &) override;
  void visit(SetItem &) override;
  void visit(RChar &) override;
  void visit(AnyExp &) override;
};

#endif // PRINTVISITOR_H__
