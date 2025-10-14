#include "libbearpig/regexparser.h"
#include "libbearpig/regextokens.h"
#include <algorithm>
#include <cassert>
#include <span>
#include <spdlog/spdlog.h>

namespace {

void print_expected(std::string_view func, RegexTokenType expected, RegexTokenType actual) {
    spdlog::debug("{}::expecting {}, current_token: {}", func,
               to_string(expected), to_string(actual));
}

} // unnamed ns

void RegexParser::end_of_input_error() {
  print_error_message_and_exit(
      fmt::format("Parser unexpectedly reached end of input"),
      current_token_idx);
}

void RegexParser::unexpected_token_error(std::string_view func,
                                         const RegexTokenType &expected) {
  print_error_message_and_exit(
      fmt::format("Unexpected token at {} in {}: got {}, expected: {}", current_token_idx, func,
                  to_string(current_token.tokentype), to_string(expected)),
      current_token_idx);
}

void RegexParser::print_error_message_and_exit(const std::string &msg,
                                               int loc) {
  std::stringstream ss;
  std::for_each(tokenstream.cbegin(), tokenstream.cend(),
                [&ss](RegexToken t) { ss << t.data; });
  spdlog::error(msg);
  spdlog::error(ss.str());
  spdlog::error("\033[31m{:~>{}}\033[0m", "^", loc+1);
  exit(1);
}

void RegexParser::advance() {
  spdlog::info("{}::current_token: {} idx: {}", __func__,
               to_string(current_token.tokentype), current_token_idx);
  current_token_idx++;
  if (current_token_idx < tokenstream.size()) {
    current_token = tokenstream.at(current_token_idx);
  } else {
    spdlog::info("is this being run?");
    current_token = RegexToken{RegexTokenType::EOS, current_token_idx, ""};
  }
}

void RegexParser::consume(RegexTokenType expected) {
  consume("<missing>", expected);
}

void RegexParser::consume(std::string_view func, RegexTokenType expected) {
  print_expected(func, expected, current_token.tokentype);
  if (current_token.tokentype == expected) {
    advance();
  } else {
    unexpected_token_error(func, expected);
  }
}

#define consume_wf(expected) (consume((__func__), (expected)))

const RegexToken &RegexParser::peek() {
  if (current_token_idx + 1 < tokenstream.size()) {
    return tokenstream.at(current_token_idx);
  }
  return invalid_token();
}

bool RegexParser::parse() {
  spdlog::info("{}::current_token: {} idx: {}", __func__,
               to_string(current_token.tokentype), current_token_idx);
  return parse_top_level();
}

bool RegexParser::parse_top_level() {
  AlternativeExp exp = parse_exp();
  assert(current_token.tokentype == RegexTokenType::EOS);
  expression_top = exp;
  return true;
}
AlternativeExp RegexParser::parse_exp() {
  return parse_alternative();
  // return true;
}
AlternativeExp RegexParser::parse_alternative() {
  AlternativeExp alternative;
  ConcatExp first = parse_simple_exp();
  alternative.alternatives.push_back(first);
  // spdlog::info("{}::success = {}", __func__, concatexp);
  if (current_token.tokentype == RegexTokenType::ALTERNATIVE) {
    consume_wf(RegexTokenType::ALTERNATIVE);
    auto next= parse_alternative();
    for(ConcatExp e: next.alternatives){
      alternative.alternatives.push_back(e);
    }
  }
  return alternative;
}

ConcatExp RegexParser::parse_simple_exp() { return parse_concatenation_exp(); }

ConcatExp RegexParser::parse_concatenation_exp() {
  std::vector<RegexTokenType> types = {
      RegexTokenType::PAREN_OPEN, RegexTokenType::SQUARE_OPEN,
      RegexTokenType::ANY, RegexTokenType::CHARACTER};
  // bool success = parse_quantified_exp();
  QuantifiedExp quantified_exp = parse_quantified_exp();
  ConcatExp concat;
  concat.exps.push_back(quantified_exp);
  // spdlog::info("{}::success = {}", __func__, success);
  if (current_token.tokentype == RegexTokenType::EOS)
    return concat;
  while(std::ranges::any_of(types, [this](RegexTokenType type) {
        return current_token.tokentype == type;
      })) {
    concat.merge(parse_concatenation_exp());
  }
  return concat;
}
QuantifiedExp RegexParser::parse_quantified_exp() {
  QuantifiedExp quantified_exp{};
  ElementaryExp elem = parse_elementary_exp();
  quantified_exp.exp = elem;
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
  // return elem;
}

ElementaryExp RegexParser::parse_elementary_exp() {
  spdlog::info("{}::current_token: {} ({}) at {}", __func__, current_token.data,
               to_string(current_token.tokentype), current_token.column);
  ElementaryExp exp;
  switch (current_token.tokentype) {
  case (RegexTokenType::PAREN_OPEN): {
    return exp;
    // return parse_group();
    break;
  }
  case (RegexTokenType::ANY): {
    return exp;
    // return parse_any();
    break;
  }
  case (RegexTokenType::SQUARE_OPEN): {
    return exp;
    // return parse_set();
    break;
  }
  case (RegexTokenType::CHARACTER): {
    RChar ch = parse_character();
    for(RegexToken tok: ch.characters){
      spdlog::info("{}", tok.data);
    }
    return ch;
    // while (current_token.tokentype == RegexTokenType::CHARACTER) {
    //   consume(__func__, RegexTokenType::CHARACTER);
    // }
    // return true;
    break;
  }
  default:
    unexpected_token_error(__func__, RegexTokenType::ANY);
    return exp;
    // return false;
  }
}

RChar RegexParser::parse_character() {
  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::CHARACTER),
               to_string(current_token.tokentype));
  RChar character;
  character.start = current_token_idx;
  while (current_token.tokentype == RegexTokenType::CHARACTER) {
    consume_wf(RegexTokenType::CHARACTER);
  }
  character.end = current_token_idx;
  character.characters = std::span<RegexToken>{tokenstream.begin()+character.start, tokenstream.begin()+character.end};
  // consume_wf(RegexTokenType::CHARACTER);
  return character;
  // return true;
}

bool RegexParser::parse_any() {
  consume_wf(RegexTokenType::ANY);
  return true;
}
bool RegexParser::parse_group() {
  consume_wf(RegexTokenType::PAREN_OPEN);

  parse_exp();

  consume_wf(RegexTokenType::PAREN_CLOSE);
  return true;
}

bool RegexParser::parse_set() {
  consume_wf(RegexTokenType::SQUARE_OPEN);

  bool negative = false;
  if (current_token.tokentype == RegexTokenType::CARET) {
    consume_wf(RegexTokenType::CARET);
    negative = true;
  }
  parse_set_items();

  consume_wf(RegexTokenType::SQUARE_CLOSE);
  spdlog::info("{}::successfully parsed a {}set", __func__,
               negative ? "negative " : "");
  return true;
}

bool RegexParser::parse_set_items() {
  while (current_token.tokentype == RegexTokenType::CHARACTER) {
    parse_set_item();
  }
  return true;
}
bool RegexParser::parse_set_item() {
  consume_wf(RegexTokenType::CHARACTER);
  if (current_token.tokentype == RegexTokenType::DASH) {
    consume_wf(RegexTokenType::DASH);
    consume_wf(RegexTokenType::CHARACTER);
  }
  return true;
}
