#ifndef PRINTVISITOR_H__
#define PRINTVISITOR_H__

#include "libbearpig/regexast.h"

namespace bp {

struct PrintVisitor {
  int depth{0};
  void operator()(this PrintVisitor &self, AlternativeExp &exp);
  void operator()(this PrintVisitor &self, ConcatExp &exp);
  void operator()(this PrintVisitor &self, QuantifiedExp &exp);
  void operator()(this PrintVisitor &self, GroupExp &exp);
  void operator()(this PrintVisitor &self, SetExp &exp);
  void operator()(this PrintVisitor &self, SetItem &exp);
  void operator()(this PrintVisitor &self, RChar &exp);
  void operator()(this PrintVisitor &self, AnyExp &exp);
};

} // namespace bp

#endif // PRINTVISITOR_H__
