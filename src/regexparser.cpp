#include "libbearpig/regexparser.h"
#include "libbearpig/regexast.h"
#include "libbearpig/regextokens.h"
#include <algorithm>
#include <memory>
#include <spdlog/spdlog.h>

namespace {

void print_expected(std::string_view func, RegexTokenType expected,
                    RegexTokenType actual) {
  spdlog::debug("{}::expecting {}, current_token: {}", func,
                to_string(expected), to_string(actual));
}

} // unnamed ns

void RegexParser::end_of_input_error() {
  print_error_message_and_exit(
      fmt::format("Parser unexpectedly reached end of input"),
      current_token_idx);
}

template <typename... Rs>
void RegexParser::unexpected_token_error(std::string_view func,
                                         Rs &&...expected) {
  std::ostringstream oss;
  (oss << ... << fmt::format("{} ", to_string(expected)));

  print_error_message_and_exit(
      fmt::format("Unexpected token at {} in {}: got {}, expected: {}",
                  current_token_idx, func, to_string(current_token.tokentype),
                  oss.str()),
      current_token_idx);
}

void RegexParser::unexpected_token_error(std::string_view func,
                                         const RegexTokenType &expected) {
  print_error_message_and_exit(
      fmt::format("Unexpected token at {} in {}: got {}, expected: {}",
                  current_token_idx, func, to_string(current_token.tokentype),
                  to_string(expected)),
      current_token_idx);
}

void RegexParser::print_error_message_and_exit(const std::string &msg,
                                               int loc) {
  std::stringstream ss;
  std::for_each(tokenstream.cbegin(), tokenstream.cend(),
                [&ss](RegexToken t) { ss << t.data; });
  spdlog::error(msg);
  spdlog::error(ss.str());
  spdlog::error("\033[31m{:~>{}}\033[0m", "^", loc + 1);
  exit(1);
}

void RegexParser::advance() {
  spdlog::debug("{}::current_token: {} idx: {}", __func__,
               to_string(current_token.tokentype), current_token_idx);
  current_token_idx++;
  if (current_token_idx < tokenstream.size()) {
    current_token = tokenstream.at(current_token_idx);
  } else {
    spdlog::debug("terminating");
    current_token = RegexToken{RegexTokenType::EOS, current_token_idx, 0};
  }
}

void RegexParser::consume(RegexTokenType expected) {
  consume("<missing>", expected);
}

void RegexParser::consume(std::string_view func, RegexTokenType expected) {
  print_expected(func, expected, current_token.tokentype);
  if (current_token.tokentype == expected or expected == RegexTokenType::ACCEPT_ANY) {
    advance();
  } else {
    unexpected_token_error(func, expected);
  }
}

#define consume_wf(expected) (consume((__func__), (expected)))

bool RegexParser::parse() {
  spdlog::debug("{}::current_token: {} idx: {}", __func__,
               to_string(current_token.tokentype), current_token_idx);
  return parse_top_level();
}

bool RegexParser::parse_top_level() {
  std::unique_ptr<AlternativeExp> exp =
      std::make_unique<AlternativeExp>(parse_exp());
  if(current_token.tokentype != RegexTokenType::EOS || !is_done()) unexpected_token_error(__func__, RegexTokenType::EOS);
  expression_top.swap(exp);
  return true;
}

AlternativeExp RegexParser::parse_exp() { return parse_alternative(); }

AlternativeExp RegexParser::parse_alternative() {
  AlternativeExp alternative;
  ConcatExp first = parse_simple_exp();
  alternative.alternatives.push_back(std::move(first));

  if (current_token.tokentype == RegexTokenType::ALTERNATIVE) {
    consume_wf(RegexTokenType::ALTERNATIVE);
    AlternativeExp next = parse_alternative();
    for (ConcatExp &e : next.alternatives) {
      alternative.alternatives.push_back(std::move(e));
    }
  }
  return alternative;
}

ConcatExp RegexParser::parse_simple_exp() { return parse_concatenation_exp(); }

ConcatExp RegexParser::parse_concatenation_exp() {
  std::vector<RegexTokenType> types = {
      RegexTokenType::PAREN_OPEN, RegexTokenType::SQUARE_OPEN,
      RegexTokenType::ANY, RegexTokenType::CHARACTER, RegexTokenType::ESCAPE};
  QuantifiedExp quantified_exp = parse_quantified_exp();
  ConcatExp concat;
  concat.exps.push_back(std::move(quantified_exp));
  if (current_token.tokentype == RegexTokenType::EOS)
    return concat;
  while (std::ranges::any_of(types, [this](RegexTokenType type) {
    return current_token.tokentype == type;
  })) {
    ConcatExp next_concat = parse_concatenation_exp();
    concat.merge(next_concat);
  }
  return concat;
}

QuantifiedExp RegexParser::parse_quantified_exp() {
  QuantifiedExp quantified_exp{};
  auto e = parse_elementary_exp();
  quantified_exp.exp = std::move(e);
  switch (current_token.tokentype) {
  case (RegexTokenType::STAR): {
    consume_wf(RegexTokenType::STAR);
    quantified_exp.quantifier = QuantifiedExp::Quantifier::STAR;
    break;
  }
  case (RegexTokenType::PLUS): {
    consume_wf(RegexTokenType::PLUS);
    quantified_exp.quantifier = QuantifiedExp::Quantifier::PLUS;
    break;
  }
  case (RegexTokenType::OPTIONAL): {
    consume_wf(RegexTokenType::OPTIONAL);
    quantified_exp.quantifier = QuantifiedExp::Quantifier::OPTIONAL;
    break;
  }
  default:
    // It's okay to not have a quantifier
    break;
  }

  return quantified_exp;
}

std::unique_ptr<ElementaryExp> RegexParser::parse_elementary_exp() {
  spdlog::debug("{}::current_token: {} ({}) at {}", __func__, current_token.data,
               to_string(current_token.tokentype), current_token.column);
  switch (current_token.tokentype) {
  case (RegexTokenType::PAREN_OPEN): {
    return std::make_unique<GroupExp>(parse_group());
  }
  case (RegexTokenType::ANY): {
    return std::make_unique<AnyExp>(parse_any());
  }
  case (RegexTokenType::SQUARE_OPEN): {
    return std::make_unique<SetExp>(parse_set());
  }
  case (RegexTokenType::CHARACTER): {
    return std::make_unique<RChar>(parse_character());
  }
  case (RegexTokenType::ESCAPE): {
    return std::make_unique<EscapeSeq>(parse_escape_seq());
  }
  default:
    unexpected_token_error(__func__, RegexTokenType::PAREN_OPEN,
                           RegexTokenType::SQUARE_OPEN,
                           RegexTokenType::CHARACTER, RegexTokenType::ANY);
    return {};
  }
}

RChar RegexParser::parse_character(bool single) {
  spdlog::debug("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::CHARACTER),
               to_string(current_token.tokentype));
  RChar character;
  character.idx = current_token_idx;
  character.character = tokenstream.at(current_token_idx);
  if(current_token.tokentype == RegexTokenType::ESCAPE) return parse_escape_seq();
  consume_wf(RegexTokenType::CHARACTER);
  return character;
}

EscapeSeq RegexParser::parse_escape_seq() {
  spdlog::debug("{}::current {} next: {}", __func__,
               to_string(current_token.tokentype),
               to_string(tokenstream.at(current_token_idx + 1).tokentype));
  EscapeSeq esc;
  esc.idx = current_token_idx;
  consume_wf(RegexTokenType::ESCAPE);
  auto escaped_token = current_token;
  esc.character = escaped_token;
  consume_wf(RegexTokenType::ACCEPT_ANY);
  spdlog::debug("{}::escaped token: '{}' ({})", __func__, escaped_token.data, to_string(escaped_token.tokentype));
  return esc;
}

AnyExp RegexParser::parse_any() {
  consume_wf(RegexTokenType::ANY);
  return AnyExp{};
}

GroupExp RegexParser::parse_group() {
  consume_wf(RegexTokenType::PAREN_OPEN);

  GroupExp group;
  group.subExp = std::make_unique<AlternativeExp>(parse_exp());

  consume_wf(RegexTokenType::PAREN_CLOSE);
  return group;
}

SetExp RegexParser::parse_set() {
  spdlog::debug("{}::attempting to parse set", __func__);
  consume_wf(RegexTokenType::SQUARE_OPEN);
  SetExp e;

  if (current_token.tokentype == RegexTokenType::CARET) {
    consume_wf(RegexTokenType::CARET);
    e.negative = true;
  }
  e.items = std::move(parse_set_items());

  consume_wf(RegexTokenType::SQUARE_CLOSE);
  spdlog::debug("{}::successfully parsed a {}set", __func__,
               e.negative ? "negative " : "");
  return e;
}

std::vector<SetItem> RegexParser::parse_set_items() {
  std::vector<SetItem> items;
  while (current_token.tokentype == RegexTokenType::CHARACTER || current_token.tokentype == RegexTokenType::ESCAPE) {
    items.emplace_back(std::move(parse_set_item()));
  }
  return items;
}

SetItem RegexParser::parse_set_item() {
  spdlog::debug("{}::attempting to a parse set item!", __func__);
  SetItem item;

  item.start = std::move(parse_character());
  if (current_token.tokentype == RegexTokenType::DASH) {
    consume_wf(RegexTokenType::DASH);
    item.range = true;
    item.stop = parse_character(true);
  }
  spdlog::debug("{}::parsed a {}set item! start:{}{}", __func__,
               item.range ? "range " : "", item.start.to_string(),
               item.range ? fmt::format("-{}", item.stop.to_string()) : "");
  return item;
}
