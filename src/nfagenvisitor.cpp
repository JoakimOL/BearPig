#include "libbearpig/regextokens.h"
#include "spdlog/spdlog.h"
#include <libbearpig/nfa.h>
#include <libbearpig/nfagenvisitor.h>
#include <libbearpig/regexast.h>

void NfaGenVisitor::print_diag_message(const std::string &msg, size_t start,
                                       size_t stop,
                                       spdlog::level::level_enum level) {
  std::stringstream ss;
  // 31 = red for error, 33 = yellow for warning
  std::string color = level == spdlog::level::err ? "31" : "33";
  std::for_each(tokenstream.cbegin(), tokenstream.cend(),
                [&ss](RegexToken t) { ss << t.data; });
  spdlog::log(level, msg);
  spdlog::log(level, ss.str());
  std::string padding = fmt::format("{: >{}}", " ", start);
  std::string diag_line = fmt::format("{:^>{}}", "^", stop - start + 1);
  spdlog::log(level, "\033[{}m{}{}\033[0m", color, padding, diag_line);
}

void NfaGenVisitor::invalid_range_error(RChar startchar, RChar stopchar) {

  print_diag_message(fmt::format("[{}-{}] is an invalid range", startchar.character.data, stopchar.character.data),
                     startchar.character.column, stopchar.character.column);
  exit(1);
}

void NfaGenVisitor::confusing_range_warning(RChar startchar, RChar stopchar) {
  auto columnstart = startchar.character.column;
  auto columnstop = stopchar.character.column;
  print_diag_message(
      fmt::format("[{}-{}] might not do what you expected. Consider not mixing "
                  "upper case and lower case symbols in the same range",
                  startchar.character.data, stopchar.character.data),
      columnstart, columnstop, spdlog::level::warn);
}

void NfaGenVisitor::visit(AlternativeExp &exp) {

  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  size_t start = nfa.add_state();
  size_t parent_id = id;
  size_t end = nfa.add_state();
  nfa.add_transition_to_state(parent_id, start, 0);
  if (nfa.currentAccept.id == parent_id) {
    spdlog::debug("accepting state was: {}, current:{}", nfa.currentAccept.id,
                  end);
    nfa.currentAccept.is_accept = false;
    nfa.currentAccept = nfa.states.at(end);
    nfa.states.at(end).is_accept = true;
  }
  id = start;
  for (size_t i = 0; i < exp.alternatives.size(); i++) {
    size_t new_state = nfa.add_state();
    nfa.add_transition_to_state(start, new_state, 0);
    // nfa.add_transition_to_state(start, new_state, '1');
    id = new_state;
    exp.alternatives[i].apply(this);
    nfa.add_transition_to_state(id, end, 0);
    // nfa.add_transition_to_state(id, end, 'e');
  }
  id = end;
  return;
}

void NfaGenVisitor::visit(ConcatExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);

  size_t start = nfa.add_state();
  size_t parent_id = id;
  // nfa.add_transition_to_state(parent_id, start, '2');
  nfa.add_transition_to_state(parent_id, start, 0);
  id = start;
  for (size_t i = 0; i < exp.exps.size(); i++) {
    exp.exps[i].apply(this);
  }
  return;
}

void NfaGenVisitor::visit(QuantifiedExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  size_t start = id;
  size_t end = nfa.add_state();

  switch (exp.quantifier) {
  case QuantifiedExp::Quantifier::NONE: {
    exp.exp->apply(this);
    nfa.add_transition_to_state(id, end, 0);
    break;
  };
  case QuantifiedExp::Quantifier::STAR: {
    exp.exp->apply(this);
    nfa.add_transition_to_state(id, start, 0);
    nfa.add_transition_to_state(start, end, 0);
    nfa.add_transition_to_state(id, end, 0);
    break;
  }
  case QuantifiedExp::Quantifier::PLUS: {
    exp.exp->apply(this);
    nfa.add_transition_to_state(id, start, 0);
    nfa.add_transition_to_state(id, end, 0);
    break;
  };
  case QuantifiedExp::Quantifier::OPTIONAL: {
    exp.exp->apply(this);
    nfa.add_transition_to_state(id, end, 0);
    nfa.add_transition_to_state(start, end, 0);
    break;
  }
  }
  id = end;

  return;
}

void NfaGenVisitor::visit(GroupExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  // size_t subexpstart = nfa.add_state();
  exp.subExp->apply(this);
  return;
}

void NfaGenVisitor::visit(SetExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  size_t start = nfa.add_state();
  nfa.add_transition_to_state(id, start, 0);
  size_t end = nfa.add_state();
  id = start;
  for (auto item : exp.items) {
    size_t new_state = nfa.add_state();
    nfa.add_transition_to_state(start, new_state, 0);
    id = new_state;
    item.apply(this);
    nfa.add_transition_to_state(id, end, 0);
  }
  id = end;

  return;
}

void NfaGenVisitor::visit(SetItem &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  size_t subexpstart = nfa.add_state();
  size_t end = nfa.add_state();
  size_t parent_id = id;
  id = subexpstart;
  nfa.add_transition_to_state(parent_id, subexpstart, 0);
  if (exp.range) {
    char startchar = exp.start.character.data;
    char stopchar = exp.stop.character.data;
    if (startchar > stopchar) {
      invalid_range_error(exp.start, exp.stop);
    }
    else if (stopchar >= 91 && startchar <= 96) {
      confusing_range_warning(exp.start, exp.stop);
    }
    char diff = stopchar - startchar;
    for (char i = 0; i <= diff; i++) {
      size_t new_state = nfa.add_state();
      nfa.add_transition_to_state(subexpstart, new_state, 0);
      // nfa.add_transition_to_state(subexpstart, new_state, '1');
      id = new_state;
      RChar synth;
      synth.character = RegexToken{RegexTokenType::CHARACTER, exp.start.idx,
                                   static_cast<char>(startchar + i)};
      synth.idx = exp.start.idx;
      synth.apply(this);
      // nfa.add_transition_to_state(id, end, 'e');
      nfa.add_transition_to_state(id, end, 0);
    }
  }
  else {
    exp.start.apply(this);
    // nfa.add_transition_to_state(id, end, 'e');
    nfa.add_transition_to_state(id, end, 0);
  }
  id = end;
  return;
}

void NfaGenVisitor::visit(RChar &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  size_t subexpstart = nfa.add_state();
  size_t parent_id = id;
  nfa.add_transition_to_state(parent_id, subexpstart, exp.character.data);
  last_char = exp.character.data;
  id = subexpstart;
  return;
}

void NfaGenVisitor::visit(AnyExp &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  // not quite sure how to implement this yet
  // it seems dependant on how i implement the traversal and how i consume
  // characters
  return;
}

void NfaGenVisitor::visit(EscapeSeq &exp) {
  spdlog::debug("{}! parent: {}", __PRETTY_FUNCTION__, id);
  size_t subexpstart = nfa.add_state();
  size_t parent_id = id;
  nfa.add_transition_to_state(parent_id, subexpstart, exp.character.data);
  last_char = exp.character.data;
  id = subexpstart;
  return;
}
