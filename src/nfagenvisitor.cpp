#include "libbearpig/regextokens.h"
#include "spdlog/spdlog.h"
#include <libbearpig/nfa.h>
#include <libbearpig/nfagenvisitor.h>
#include <libbearpig/regexast.h>
#include <algorithm>

namespace {
void print_diag_message(const std::string &msg,
                        std::vector<bp::RegexToken> tokenstream, size_t start,
                        size_t stop, spdlog::level::level_enum level) {
  std::stringstream ss;
  // 31 = red for error, 33 = yellow for warning
  std::string color = level == spdlog::level::err ? "31" : "33";
  std::for_each(tokenstream.cbegin(), tokenstream.cend(),
                [&ss](bp::RegexToken t) { ss << t.data; });
  spdlog::log(level, msg);
  spdlog::log(level, ss.str());
  std::string padding = fmt::format("{: >{}}", " ", start);
  std::string diag_line = fmt::format("{:^>{}}", "^", stop - start + 1);
  spdlog::log(level, "\033[{}m{}{}\033[0m", color, padding, diag_line);
}
} // namespace

namespace bp {

void NfaGenVisitor::invalid_range_error(RChar startchar, RChar stopchar) {

  print_diag_message(fmt::format("[{}-{}] is an invalid range",
                                 startchar.character.data,
                                 stopchar.character.data),
                     tokenstream, startchar.character.column, stopchar.character.column, spdlog::level::err);
  exit(1);
}

void NfaGenVisitor::confusing_range_warning(RChar startchar, RChar stopchar) {
  auto columnstart = startchar.character.column;
  auto columnstop = stopchar.character.column;
  print_diag_message(
      fmt::format("[{}-{}] might not do what you expected. Consider not mixing "
                  "upper case and lower case symbols in the same range",
                  startchar.character.data, stopchar.character.data),
      tokenstream, columnstart, columnstop, spdlog::level::warn);
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, AlternativeExp &exp) {

  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  size_t parent_id = self.id;
  size_t end = self.nfa.add_state();
  if (self.nfa.currentAccept.id == parent_id) {
    spdlog::debug("accepting state was: {}, current:{}", self.nfa.currentAccept.id,
                  end);
    self.nfa.currentAccept.is_accept = false;
    self.nfa.currentAccept = self.nfa.states.at(end);
    self.nfa.states.at(end).is_accept = true;
  }
  for (size_t i = 0; i < exp.alternatives.size(); i++) {
    size_t new_state = self.nfa.add_state();
    self.nfa.add_transition_to_state(parent_id, new_state, 0);
    self.id = new_state;
    self(exp.alternatives[i]);
    self.nfa.add_transition_to_state(self.id, end, 0);
  }
  self.id = end;
  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, ConcatExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);

  // nfa.add_transition_to_state(parent_id, start, '2');
  for (size_t i = 0; i < exp.exps.size(); i++) {
    self(exp.exps[i]);
  }
  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor &self, QuantifiedExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  size_t start = self.id;
  size_t end = self.nfa.add_state();

  switch (exp.quantifier) {
  case QuantifiedExp::Quantifier::NONE: {
    std::visit(self, exp.exp);
    // exp.exp->apply(this);
    self.nfa.add_transition_to_state(self.id, end, 0);
    break;
  };
  case QuantifiedExp::Quantifier::STAR: {
    std::visit(self, exp.exp);
    self.nfa.add_transition_to_state(self.id, start, 0);
    self.nfa.add_transition_to_state(start, end, 0);
    self.nfa.add_transition_to_state(self.id, end, 0);
    break;
  }
  case QuantifiedExp::Quantifier::PLUS: {
    std::visit(self, exp.exp);
    self.nfa.add_transition_to_state(self.id, start, 0);
    self.nfa.add_transition_to_state(self.id, end, 0);
    break;
  };
  case QuantifiedExp::Quantifier::OPTIONAL: {
    std::visit(self, exp.exp);
    self.nfa.add_transition_to_state(self.id, end, 0);
    self.nfa.add_transition_to_state(start, end, 0);
    break;
  }
  }
  self.id = end;

  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, GroupExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  // size_t subexpstart = nfa.add_state();
  // exp.subExp->apply(this);
  self(*exp.subExp);
  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, SetExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  size_t start = self.nfa.add_state();
  self.nfa.add_transition_to_state(self.id, start, 0);
  size_t end = self.nfa.add_state();
  self.id = start;
  for (auto item : exp.items) {
    size_t new_state = self.nfa.add_state();
    self.nfa.add_transition_to_state(start, new_state, 0);
    self.id = new_state;
    // item.apply(this);
    self(item);
    self.nfa.add_transition_to_state(self.id, end, 0);
  }
  self.id = end;

  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, SetItem &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  size_t subexpstart = self.nfa.add_state();
  size_t end = self.nfa.add_state();
  size_t parent_id = self.id;
  self.id = subexpstart;
  self.nfa.add_transition_to_state(parent_id, subexpstart, 0);
  if (exp.range) {
    char startchar = exp.start.character.data;
    char stopchar = exp.stop.character.data;
    if (startchar > stopchar) {
      self.invalid_range_error(exp.start, exp.stop);
    } else if (stopchar >= 91 && startchar <= 96) {
      self.confusing_range_warning(exp.start, exp.stop);
    }
    char diff = stopchar - startchar;
    for (char i = 0; i <= diff; i++) {
      size_t new_state = self.nfa.add_state();
      self.nfa.add_transition_to_state(subexpstart, new_state, 0);
      // self.nfa.add_transition_to_state(subexpstart, new_state, '1');
      self.id = new_state;
      RChar synth;
      synth.character = RegexToken{RegexTokenType::CHARACTER, exp.start.idx,
                                   static_cast<char>(startchar + i)};
      synth.idx = exp.start.idx;
      self(synth);
      // synth.apply(this);
      // self.nfa.add_transition_to_state(id, end, 'e');
      self.nfa.add_transition_to_state(self.id, end, 0);
    }
  } else {
    self(exp.start);
    // exp.start.apply(this);
    // self.nfa.add_transition_to_state(id, end, 'e');
    self.nfa.add_transition_to_state(self.id, end, 0);
  }
  self.id = end;
  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, RChar &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  size_t subexpstart = self.nfa.add_state();
  size_t parent_id = self.id;
  self.nfa.add_transition_to_state(parent_id, subexpstart, exp.character.data);
  self.last_char = exp.character.data;
  self.id = subexpstart;
  return;
}

void NfaGenVisitor::operator()(this NfaGenVisitor& self, AnyExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, self.id);
  // not quite sure how to implement this yet
  // it seems dependant on how i implement the traversal and how i consume
  // characters
  size_t subexpstart = self.nfa.add_state();
  size_t parent_id = self.id;
  self.nfa.add_transition_to_state(parent_id, subexpstart, 1);
  self.id = subexpstart;
  return;
}
} // namespace bp
