#ifndef NFAGENVISITOR_H_
#define NFAGENVISITOR_H_

#include "libbearpig/nfa.h"
#include "libbearpig/regexast.h"
#include "libbearpig/regextokens.h"
#include <vector>

namespace bp {

struct NfaGenVisitor {
private:
  NFA &nfa;
  std::vector<RegexToken> tokenstream;
  size_t id{0};
  char last_char;

public:
  NfaGenVisitor(NFA &nfa, std::vector<RegexToken> tokens)
      : nfa{nfa}, tokenstream{tokens} {};

  void invalid_range_error(RChar startchar, RChar stopchar);
  void confusing_range_warning(RChar start, RChar stop);

  void operator()(this NfaGenVisitor &self, AlternativeExp &exp);
  void operator()(this NfaGenVisitor &self, ConcatExp &exp);
  void operator()(this NfaGenVisitor &self, QuantifiedExp &exp);
  void operator()(this NfaGenVisitor &self, GroupExp &exp);
  void operator()(this NfaGenVisitor &self, SetExp &exp);
  void operator()(this NfaGenVisitor &self, SetItem &exp);
  void operator()(this NfaGenVisitor &self, RChar &exp);
  void operator()(this NfaGenVisitor &self, AnyExp &exp);
};

} // namespace bp

#endif // NFAGENVISITOR_H_
