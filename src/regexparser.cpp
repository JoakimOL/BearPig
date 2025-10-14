#include "libbearpig/regexparser.h"
#include "libbearpig/regextokens.h"
#include <algorithm>
#include <cassert>
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
  bool success = parse_exp();
  assert(current_token.tokentype == RegexTokenType::EOS);
  return success;
}
bool RegexParser::parse_exp() { return parse_alternative(); }
bool RegexParser::parse_alternative() {
  bool success = parse_simple_exp();
  spdlog::info("{}::success = {}", __func__, success);
  if (success && current_token.tokentype == RegexTokenType::ALTERNATIVE) {
    consume_wf(RegexTokenType::ALTERNATIVE);
    return parse_alternative();
  }
  return success;
}
bool RegexParser::parse_simple_exp() { return parse_concatenation_exp(); }
bool RegexParser::parse_concatenation_exp() {
  std::vector<RegexTokenType> types = {
      RegexTokenType::PAREN_OPEN, RegexTokenType::SQUARE_OPEN,
      RegexTokenType::ANY, RegexTokenType::CHARACTER};
  bool success = parse_quantified_exp();
  spdlog::info("{}::success = {}", __func__, success);
  if (current_token.tokentype == RegexTokenType::EOS)
    return success;
  if (std::ranges::any_of(types, [this](RegexTokenType type) {
        return current_token.tokentype == type;
      })) {
    return parse_concatenation_exp();
  }
  return success;
}
bool RegexParser::parse_quantified_exp() {
  bool elem = parse_elementary_exp();
  switch (current_token.tokentype) {
  case (RegexTokenType::STAR): {
    consume_wf(RegexTokenType::STAR);
    break;
  }
  case (RegexTokenType::PLUS): {
    consume_wf(RegexTokenType::PLUS);
    break;
  }
  case (RegexTokenType::OPTIONAL): {
    consume_wf(RegexTokenType::OPTIONAL);
    break;
  }
  default:
    break;
  }

  return elem;
}

bool RegexParser::parse_elementary_exp() {
  spdlog::info("{}::current_token: {} ({}) at {}", __func__, current_token.data,
               to_string(current_token.tokentype), current_token.column);
  switch (current_token.tokentype) {
  case (RegexTokenType::PAREN_OPEN): {
    return parse_group();
    break;
  }
  case (RegexTokenType::ANY): {
    return parse_any();
    break;
  }
  case (RegexTokenType::SQUARE_OPEN): {
    return parse_set();
    break;
  }
  case (RegexTokenType::CHARACTER): {
    while (current_token.tokentype == RegexTokenType::CHARACTER) {
      consume(__func__, RegexTokenType::CHARACTER);
    }
    return true;
    break;
  }
  default:
    return false;
  }
}

bool RegexParser::parse_character() {
  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::CHARACTER),
               to_string(current_token.tokentype));
  consume_wf(RegexTokenType::CHARACTER);
  return true;
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
