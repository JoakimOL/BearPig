#ifndef REGEXAST_H__
#define REGEXAST_H__

#include <libbearpig/regextokens.h>
#include <memory>
#include <variant>
#include <vector>

namespace bp {

struct AlternativeExp;
struct SetExp;
struct GroupExp;
struct AnyExp;
struct RChar;

// ElementaryExps arent a thing, they just form a natural group of other
// expressions
using ElementaryExp = std::variant<SetExp, GroupExp, AnyExp, RChar>;

struct AnyExp {
  ~AnyExp() = default;
};

struct RChar {
  ~RChar() = default;
  int idx;
  RegexToken character;
  bool is_escape{false};
  std::string to_string() { return std::string{character.data}; }
};

struct SetItem {
  ~SetItem() = default;
  bool range{false};
  RChar start;
  RChar stop;
};

struct SetExp {
  ~SetExp() = default;
  bool negative = false;
  std::vector<SetItem> items;
};

struct GroupExp {
  explicit GroupExp(const GroupExp &other) = delete;
  explicit GroupExp();
  ~GroupExp();
  GroupExp(GroupExp &&other);

  GroupExp &operator=(const GroupExp &other) = delete;
  GroupExp &operator=(GroupExp &&other) {
    subExp.swap(other.subExp);
    return *this;
  }
  std::unique_ptr<AlternativeExp> subExp{};
};

struct QuantifiedExp {
  QuantifiedExp(QuantifiedExp &&other)
      : exp(std::move(other.exp)), quantifier(other.quantifier) {}
  explicit QuantifiedExp(const QuantifiedExp &other) = delete;
  explicit QuantifiedExp() = default;
  QuantifiedExp &operator=(const QuantifiedExp &other) = delete;
  QuantifiedExp &operator=(QuantifiedExp &&other) {
    exp.swap(other.exp);
    return *this;
  }
  ~QuantifiedExp() = default;

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
    default:
      return "Should never happen";
    }
  }

  Quantifier quantifier = Quantifier::NONE;
  ElementaryExp exp;
};

struct ConcatExp {
  ConcatExp(ConcatExp &&other) : exps(std::move(other.exps)) {}
  explicit ConcatExp(const ConcatExp &other) = delete;
  explicit ConcatExp() = default;
  ConcatExp &operator=(const ConcatExp &other) = delete;
  ConcatExp &operator=(ConcatExp &&other) {
    exps.swap(other.exps);
    return *this;
  }
  ~ConcatExp() = default;
  std::vector<QuantifiedExp> exps;

  void merge(ConcatExp &other) {
    for (QuantifiedExp &e : other.exps) {
      exps.push_back(std::move(e));
    }
  }
};

struct AlternativeExp {
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
  std::vector<ConcatExp> alternatives{};
};
} // namespace bp

#endif // REGEXAST_H__
