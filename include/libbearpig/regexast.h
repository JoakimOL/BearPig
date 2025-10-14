#ifndef REGEXAST_H__
#define REGEXAST_H__

#include <libbearpig/basevisitor.h>
#include <libbearpig/regextokens.h>
#include <numeric>
#include <span>

class Visitable {
public:
  virtual void apply(BaseVisitor *v) = 0;
  virtual ~Visitable() = default;
};

// ElementaryExps arent a thing, they just form a natural group of other
// expressions
struct ElementaryExp : public Visitable {};

struct AnyExp : public ElementaryExp {
  ~AnyExp() = default;
  void apply(BaseVisitor *v) override;
};

struct RChar : public ElementaryExp {
  ~RChar() = default;
  void apply(BaseVisitor *v) override;
  int idx;
  RegexToken character;
  std::string to_string() { return std::string(character.data); }
};

struct EscapeSeq : public RChar {
  ~EscapeSeq() = default;
  void apply(BaseVisitor *v) override;
};

struct SetItem : public Visitable {
  ~SetItem() = default;
  void apply(BaseVisitor *v) override;
  bool range{false};
  RChar start;
  RChar stop;
};

struct SetExp : public ElementaryExp {
  ~SetExp() = default;
  void apply(BaseVisitor *v) override;
  bool negative = false;
  std::vector<SetItem> items;
};

struct GroupExp : public ElementaryExp {
  explicit GroupExp() = default;
  ~GroupExp() = default;
  explicit GroupExp(const GroupExp &other) = delete;
  GroupExp(GroupExp &&other) : subExp(std::move(other.subExp)) {}

  GroupExp &operator=(const GroupExp &other) = delete;
  GroupExp &operator=(GroupExp &&other) {
    subExp.swap(other.subExp);
    return *this;
  }
  void apply(BaseVisitor *v) override;
  std::unique_ptr<AlternativeExp> subExp{};
};

struct QuantifiedExp : public Visitable {
  QuantifiedExp(QuantifiedExp &&other) : exp(std::move(other.exp)), quantifier(other.quantifier) {}
  explicit QuantifiedExp(const QuantifiedExp &other) = delete;
  explicit QuantifiedExp() = default;
  QuantifiedExp &operator=(const QuantifiedExp &other) = delete;
  QuantifiedExp &operator=(QuantifiedExp &&other) {
    exp.swap(other.exp);
    return *this;
  }
  ~QuantifiedExp() = default;
  void apply(BaseVisitor *v) override;

  enum class Quantifier {
    NONE,
    STAR,
    PLUS,
    OPTIONAL,
  };
  static std::string to_string(Quantifier q) {
    switch (q) {
    case (QuantifiedExp::Quantifier::NONE): {
      return "NONE";
    }
    case (QuantifiedExp::Quantifier::STAR): {
      return "STAR";
    }
    case (QuantifiedExp::Quantifier::PLUS): {
      return "PLUS";
    }
    case (QuantifiedExp::Quantifier::OPTIONAL): {
      return "OPTIONAL";
    }
    }
  }

  Quantifier quantifier = Quantifier::NONE;
  std::unique_ptr<ElementaryExp> exp;
};

struct ConcatExp : public Visitable {
  ConcatExp(ConcatExp &&other) : exps(std::move(other.exps)) {}
  explicit ConcatExp(const ConcatExp &other) = delete;
  explicit ConcatExp() = default;
  ConcatExp &operator=(const ConcatExp &other) = delete;
  ConcatExp &operator=(ConcatExp &&other) {
    exps.swap(other.exps);
    return *this;
  }
  ~ConcatExp() = default;
  void apply(BaseVisitor *v) override;
  std::vector<QuantifiedExp> exps;

  void merge(ConcatExp &other) {
    for (QuantifiedExp &e : other.exps) {
      exps.push_back(std::move(e));
    }
  }
};

struct AlternativeExp : public Visitable {
  AlternativeExp(AlternativeExp &&other)
      : alternatives(std::move(other.alternatives)) {}
  explicit AlternativeExp(const AlternativeExp &other) = delete;
  explicit AlternativeExp() = default;
  AlternativeExp &operator=(const AlternativeExp &other) = delete;
  AlternativeExp &operator=(AlternativeExp &&other) {
    alternatives.swap(other.alternatives);
    return *this;
  }
  ~AlternativeExp() = default;
  void apply(BaseVisitor *v) override;
  std::vector<ConcatExp> alternatives{};
};

#endif // REGEXAST_H__
