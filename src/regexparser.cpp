#include "libbearpig/regextokens.h"
#include <algorithm>
#include <libbearpig/regexparser.h>
#include <spdlog/spdlog.h>
#include <sstream>

void RegexParser::end_of_input_error() {
  print_error_message_and_exit(
      fmt::format("Parser unexpectedly reached end of input"),
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

std::optional<RegexToken> RegexParser::get_next_token() {
  if (current_token_idx >= tokenstream.size()) {
    return std::nullopt;
    // end_of_input_error();
  }
  auto token = tokenstream.at(current_token_idx);
  current_token_idx++;
  return token;
}

std::optional<RegexToken> RegexParser::peek() {
  if (current_token_idx + 1 >= tokenstream.size()) {
    return std::nullopt;
  }
  auto token = tokenstream.at(current_token_idx + 1);
  return token;
}

bool RegexParser::expect(std::optional<RegexToken> actual,
                         RegexTokenType expected) {
  if (actual.has_value() && actual->tokentype == expected) {
    spdlog::info("\tsuccessfully parsed {}",
                 to_string(actual.value().tokentype));
    return true;
  }
  spdlog::info("\texpected {}, got {}", to_string(expected),
               to_string(actual.value().tokentype));
  return false;
}

// top level parse function
bool RegexParser::parse() { return parse_regex() && is_done(); }

bool RegexParser::parse_regex() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  auto result = parse_simple_exp();
  return result;
}

bool RegexParser::parse_simple_exp() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  int backtrack_idx = current_token_idx;

  auto concat = parse_concatenation_exp();
  if (concat) {
    spdlog::info("\tparsing concatenation_exp successful!");
    return true;
  }
  current_token_idx = backtrack_idx;

  auto quantified = parse_quantified_exp();
  if (quantified) {
    spdlog::info("\tparsing quantified_exp successful!");
    return true;
  }
  current_token_idx = backtrack_idx;

  return false;
}

bool RegexParser::parse_concatenation_exp() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  int backtrack_idx = current_token_idx;
  auto quantified_exp = parse_quantified_exp();
  // a concatenation exp needs at least one part
  if (!quantified_exp) {
    current_token_idx = backtrack_idx;
    return false;
  }

  backtrack_idx = current_token_idx;
  // the other part concatenated is optional
  auto concat_exp = parse_concatenation_exp();
  if (!concat_exp) {
    current_token_idx = backtrack_idx;
  }
  return true;
}

bool RegexParser::parse_quantified_exp() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  // parses an elementary_exp with optional quantifiers (star or plus)
  // basic-RE ::= star
  //            | plus
  //            | optional
  //            | elementary-RE
  auto elementary_exp = parse_elementary_exp();
  if (!elementary_exp) {
    spdlog::info("\tparsing elementary_exp failed!");
    return false;
  }
  spdlog::info("\tparsing elementary_exp successful! current_token: {}",
               current_token_idx);

  // auto next = peek();
  // if (next.has_value()) {
  //   switch (next->tokentype) {
  //   case RegexTokenType::STAR: {
  //     parse_single_character_token(RegexTokenType::STAR);
  //     spdlog::info("\tsuccessfully parsed star");
  //     return true;
  //   }
  //   case RegexTokenType::PLUS: {
  //     parse_single_character_token(RegexTokenType::PLUS);
  //     spdlog::info("\tsuccessfully parsed plus");
  //     return true;
  //   }
  //   case RegexTokenType::OPTIONAL: {
  //     parse_single_character_token(RegexTokenType::OPTIONAL);
  //     spdlog::info("\tsuccessfully parsed optional");
  //     return true;
  //   }
  //   default: {
  //     spdlog::info("\tthis should never happen i think");
  //     return false;
  //   }
  //   }
  // }
  int backtrack_idx = current_token_idx;

  auto star = parse_single_character_token(RegexTokenType::STAR);
  if(star){
    spdlog::info("\tsuccessfully parsed star");
    return true;
  }
  current_token_idx = backtrack_idx;

  auto plus = parse_single_character_token(RegexTokenType::PLUS);
  if(plus){
    spdlog::info("\tsuccessfully parsed plus");
    return true;
  }
  current_token_idx = backtrack_idx;

  auto optional = parse_single_character_token(RegexTokenType::OPTIONAL);
  if(optional){
    spdlog::info("\tsuccessfully parsed optional");
    return true;
  }
  current_token_idx = backtrack_idx;

  // if we're this far, we know the elementary_exp succesfully parsed.
  // Hence we return true, as per the first production rule
  spdlog::info("\tsuccessfully parsed quantified exp without quantifier");
  return true;
}

bool RegexParser::parse_elementary_exp() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  spdlog::info("\tattempting to parse group. current_token_idx: {}",
               current_token_idx);
  // <elementary-RE> ::= group
  //                   | any
  //                   | char
  //                   | set

  // Trying to parse group
  int backtrack_idx = current_token_idx;
  auto group = parse_group();
  if (group) {
    spdlog::info("\t{}", "parsing group success!");
    return true;
  }
  current_token_idx = backtrack_idx;

  // Trying to parse set
  backtrack_idx = current_token_idx;
  auto set = parse_set();
  if (set) {
    spdlog::info("\t{}", "parsing set success!");
    return true;
  }
  current_token_idx = backtrack_idx;

  // Trying to parse any
  spdlog::debug("\tattempting to parse any. current_token_idx: {}",
                current_token_idx);
  auto any = parse_any();
  if (any) {
    spdlog::info("\tparsing any success!");
    return any;
  } else {
    current_token_idx = backtrack_idx;
  }

  // Trying to parse character
  spdlog::debug("\tattempting to parse character. current_token_idx: {}",
                current_token_idx);
  auto character = parse_character();
  if (character) {
    spdlog::info("\tparsing character success!");
    return character;
  } else {
    current_token_idx = backtrack_idx;
  }
  return false;
}

bool RegexParser::parse_set_items(){
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  int backtrack_idx = current_token_idx;
  while(parse_single_character_token(RegexTokenType::CHARACTER));
  spdlog::info("Finished parsing characters at idx {}. expecting dash or closing square", current_token_idx);
  bool range = false;
  if(parse_single_character_token(RegexTokenType::DASH)){ range = true; }
  if(range){
    while(parse_single_character_token(RegexTokenType::CHARACTER));
    spdlog::info("Finished parsing characters at idx {}. expecting dash or closing square", current_token_idx);
  }
  return true;
}

bool RegexParser::parse_set() {
  // <set> ::= <positive-set>
  //         | <negative-set>
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  int backtrack_idx = current_token_idx;
  auto maybe_paren_open = get_next_token();
  RegexToken paren_open;
  if (maybe_paren_open.has_value())
    paren_open = maybe_paren_open.value();
  else
    return false;
  spdlog::info("\texpecting paren_open, got {}",
               to_string(paren_open.tokentype));
  if (paren_open.tokentype != RegexTokenType::SQUARE_OPEN) {
    current_token_idx = backtrack_idx;
    return false;
  }
  spdlog::info("\tattempting to parse body of group. current_token_idx: {}",
               current_token_idx);
  bool negative_set = false;
  if(parse_single_character_token(RegexTokenType::CARET)) {negative_set = true; }
  if (!parse_set_items()) {
    current_token_idx = backtrack_idx;
    return false;
  }
  spdlog::info("\tbody parsed successfully");
  auto maybe_paren_close = get_next_token();
  RegexToken paren_close;
  if (maybe_paren_close.has_value())
    paren_close = maybe_paren_close.value();
  else
    return false;
  spdlog::info("\texpecting paren_close, got {}",
               to_string(paren_close.tokentype));
  if (paren_close.tokentype != RegexTokenType::SQUARE_CLOSE) {
    current_token_idx = backtrack_idx;
    return false;
  }
  return true;
}

bool RegexParser::parse_group() {
  //  group ::= "(" regex ")"
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  int backtrack_idx = current_token_idx;
  auto maybe_paren_open = get_next_token();
  RegexToken paren_open;
  if (maybe_paren_open.has_value())
    paren_open = maybe_paren_open.value();
  else
    return false;
  spdlog::info("\texpecting paren_open, got {}",
               to_string(paren_open.tokentype));
  if (paren_open.tokentype != RegexTokenType::PAREN_OPEN) {
    current_token_idx = backtrack_idx;
    return false;
  }
  spdlog::info("\tattempting to parse body of group. current_token_idx: {}",
               current_token_idx);
  if (!parse_regex()) {
    current_token_idx = backtrack_idx;
    return false;
  }
  spdlog::info("\tbody parsed successfully");
  auto maybe_paren_close = get_next_token();
  RegexToken paren_close;
  if (maybe_paren_close.has_value())
    paren_close = maybe_paren_close.value();
  else
    return false;
  spdlog::info("\texpecting paren_close, got {}",
               to_string(paren_close.tokentype));
  if (paren_close.tokentype != RegexTokenType::PAREN_CLOSE) {
    current_token_idx = backtrack_idx;
    return false;
  }
  return true;
}

// XXX (jlier):
// create version of this with an array of RegexTokenTypes, so we can
// match the next character with one of several types
// useful for escaping metacharacters
bool RegexParser::parse_single_character_token(RegexTokenType type) {
  // auto next = peek();
  // if(!next.has_value()) {
  //   spdlog::info("\twhile parsing {}, reached end of input.", to_string(type));
  //   return false;
  // }
  // if(expect(next, type)){
  //   next = get_next_token();
  //   return true;
  // }
  // return false;
  int backtrack_idx = current_token_idx;
  auto maybe_token = get_next_token();
  RegexToken token;
  if (maybe_token.has_value())
    token = maybe_token.value();
  else
    return false;
  spdlog::info("\texpecting {}, got {}", to_string(type),
               to_string(token.tokentype));
  if (token.tokentype == type)
    return true;
  current_token_idx = backtrack_idx;
  return false;
}

bool RegexParser::parse_any() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  return parse_single_character_token(RegexTokenType::ANY);
}

bool RegexParser::parse_character() {
  spdlog::info("{}, current_token_idx: {}", __func__, current_token_idx);
  // auto escape = parse_single_character_token(RegexTokenType::ESCAPE);
  // if(!escape) parse_single_character_token(RegexTokenType::CHARACTER);
  return parse_single_character_token(RegexTokenType::CHARACTER);
}
