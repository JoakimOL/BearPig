#include "libbearpig/regexast.h"
#include "libbearpig/basevisitor.h"

void AlternativeExp::apply(BaseVisitor *v) {
  v->visit(*this);
  v->inc_depth();
  for_each(alternatives.begin(), alternatives.end(),
           [=](ConcatExp &c) { c.apply(v); });
  v->dec_depth();
}
void ConcatExp::apply(BaseVisitor *v) {
  v->visit(*this);
  v->inc_depth();
  for_each(exps.begin(), exps.end(), [=](QuantifiedExp &q) { q.apply(v); });
  v->dec_depth();
}

void QuantifiedExp::apply(BaseVisitor *v) {
  v->visit(*this);
  v->inc_depth();
  exp->apply(v);
  v->dec_depth();
}

void GroupExp::apply(BaseVisitor *v) {
  v->visit(*this);
  v->inc_depth();
  subExp->apply(v);
  v->dec_depth();
}

void SetExp::apply(BaseVisitor *v) {
  v->visit(*this);
  v->inc_depth();
  for_each(items.begin(), items.end(), [=](SetItem i) { i.apply(v); });
  v->dec_depth();
}

void SetItem::apply(BaseVisitor *v) { v->visit(*this); }

void RChar::apply(BaseVisitor *v) { v->visit(*this); }

void AnyExp::apply(BaseVisitor *v) { v->visit(*this); }
