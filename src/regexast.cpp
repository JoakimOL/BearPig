#include "libbearpig/regexast.h"
#include "libbearpig/basevisitor.h"

void AlternativeExp::apply(BaseVisitor *v) { v->visit(*this); }
void ConcatExp::apply(BaseVisitor *v) { v->visit(*this); }

void QuantifiedExp::apply(BaseVisitor *v) { v->visit(*this); }

void GroupExp::apply(BaseVisitor *v) { v->visit(*this); }

void SetExp::apply(BaseVisitor *v) { v->visit(*this); }

void SetItem::apply(BaseVisitor *v) { v->visit(*this); }

void RChar::apply(BaseVisitor *v) { v->visit(*this); }

void AnyExp::apply(BaseVisitor *v) { v->visit(*this); }

void EscapeSeq::apply(BaseVisitor *v) { v->visit(*this); }
