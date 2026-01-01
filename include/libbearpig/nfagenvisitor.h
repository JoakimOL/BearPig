#ifndef NFAGENVISITOR_H_
#define NFAGENVISITOR_H_

#include "libbearpig/nfa.h"
#include "spdlog/common.h"
#include <libbearpig/basevisitor.h>
#include <vector>

namespace bp {

class NfaGenVisitor : public BaseVisitor {
private:
  NFA &nfa;
  std::vector<RegexToken> tokenstream;
  size_t id{0};
  char last_char;

public:
  NfaGenVisitor(NFA &nfa, std::vector<RegexToken> tokens)
      : nfa{nfa}, tokenstream{tokens} {};
  void visit(AlternativeExp &) override;
  void visit(ConcatExp &) override;
  void visit(QuantifiedExp &) override;
  void visit(GroupExp &) override;
  void visit(SetExp &) override;
  void visit(SetItem &) override;
  void visit(RChar &) override;
  void visit(AnyExp &) override;
  void visit(EscapeSeq &) override;

  void print_diag_message(const std::string &msg, size_t start, size_t stop,
                          spdlog::level::level_enum level = spdlog::level::err);
  void invalid_range_error(RChar startchar, RChar stopchar);
  void confusing_range_warning(RChar start, RChar stop);
};

} // namespace bp

#endif // NFAGENVISITOR_H_
