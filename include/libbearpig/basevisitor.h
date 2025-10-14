#ifndef BASEVISITOR_H__
#define BASEVISITOR_H__

class AlternativeExp;
class ConcatExp;
class QuantifiedExp;
class GroupExp;
class SetExp;
class SetItem;
class RChar;
class AnyExp;
class EscapeSeq;

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
  int depth{0};
  void inc_depth() { depth++; }
  void dec_depth() { depth--; }
  virtual ~BaseVisitor() = default;
};

#endif // BASEVISITOR_H__
