#include "libbearpig/regexparser.h"
#include "libbearpig/regextokens.h"
#include <cassert>
#include <spdlog/spdlog.h>

void RegexParser::end_of_input_error() {
  print_error_message_and_exit(
      fmt::format("Parser unexpectedly reached end of input"),
      current_token_idx);
}

void RegexParser::unexpected_token_error(std::string_view func,
                                         const RegexTokenType &expected) {
  print_error_message_and_exit(
      fmt::format("Unexpected token in {}: got {}, expected: {}", func,
                  to_string(current_token.tokentype), to_string(expected)),
      current_token_idx);
}

void RegexParser::print_error_message_and_exit(const std::string &msg,
                                               int loc) {
  std::stringstream ss;
  std::for_each(tokenstream.cbegin(), tokenstream.cend(),
                [&ss](RegexToken t) { ss << t.data; });
  spdlog::error(ss.str());
  spdlog::error("{:~>{}}", "^", loc);
  spdlog::error(msg);
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
  if (current_token.tokentype == expected) {
    advance();
  } else {
    unexpected_token_error(func, expected);
  }
}

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
bool RegexParser::parse_exp() { return parse_simple_exp(); }
bool RegexParser::parse_simple_exp() { return parse_concatenation_exp(); }
bool RegexParser::parse_concatenation_exp() {
  bool success = parse_quantified_exp();
  spdlog::info("{}::success = {}", __func__, success);
  if (current_token.tokentype == RegexTokenType::EOS)
    return true;
  if (success)
    return parse_concatenation_exp();
  return false;
}
bool RegexParser::parse_quantified_exp() {
  bool elem = parse_elementary_exp();
  switch (current_token.tokentype) {
  case (RegexTokenType::STAR): {
    consume(RegexTokenType::STAR);
    break;
  }
  case (RegexTokenType::PLUS): {
    consume(RegexTokenType::PLUS);
    break;
  }
  case (RegexTokenType::OPTIONAL): {
    consume(RegexTokenType::OPTIONAL);
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
  consume(__func__, RegexTokenType::CHARACTER);
  return true;
}

bool RegexParser::parse_any() {
  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::ANY),
               to_string(current_token.tokentype));
  consume(RegexTokenType::ANY);
  return true;
}
bool RegexParser::parse_group() {
  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::PAREN_OPEN),
               to_string(current_token.tokentype));
  consume(RegexTokenType::PAREN_OPEN);

  parse_exp();

  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::PAREN_CLOSE),
               to_string(current_token.tokentype));
  consume(RegexTokenType::PAREN_CLOSE);
  return true;
}

bool RegexParser::parse_set() {
  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::SQUARE_OPEN),
               to_string(current_token.tokentype));
  consume(RegexTokenType::SQUARE_OPEN);

  bool negative = false;
  auto next = peek();
  if (next.tokentype == RegexTokenType::CARET) {
    consume(RegexTokenType::CARET);
    negative = true;
  }
  parse_set_items();

  spdlog::info("{}::expecting {}, current_token: {}", __func__,
               to_string(RegexTokenType::SQUARE_CLOSE),
               to_string(current_token.tokentype));
  consume(RegexTokenType::SQUARE_CLOSE);
  return true;
}

bool RegexParser::parse_set_items() {
  auto next = peek();
  while (next.tokentype == RegexTokenType::CHARACTER) {
    parse_set_item();
    next = peek();
  }
  return true;
}
bool RegexParser::parse_set_item() {
  consume(RegexTokenType::CHARACTER);
  auto next = peek();
  if (next.tokentype == RegexTokenType::DASH) {
    consume(RegexTokenType::DASH);
    consume(RegexTokenType::CHARACTER);
  }
  return true;
}
