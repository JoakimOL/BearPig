#ifndef REGEXTOKENS_H_
#define REGEXTOKENS_H_

#include <array>
#include <string_view>
#include <string>

enum class RegexTokenType {
  // Braces
  PAREN_OPEN,
  PAREN_CLOSE,
  CURLY_OPEN,
  CURLY_CLOSE,
  SQUARE_OPEN,
  SQUARE_CLOSE,

  // Quantifiers
  STAR, // a*
  PLUS, // a+
  OPTIONAL, // a?

  // Choice
  ALTERNATIVE, // a | b
  DASH, // [a-b] set ranges
  CARET, // [^a-b] for negative sets

  // Characters
  ANY,
  WHITESPACE,
  CHARACTER,
  ESCAPE,

  EOS,
  // maybe a good idea?
  INVALID,
};

static std::array metacharacters = {
  RegexTokenType::PLUS,
  RegexTokenType::PAREN_OPEN,
  RegexTokenType::PAREN_CLOSE,
  RegexTokenType::CURLY_OPEN,
  RegexTokenType::CURLY_CLOSE,
  RegexTokenType::SQUARE_OPEN,
  RegexTokenType::SQUARE_CLOSE,
  RegexTokenType::STAR,
  RegexTokenType::PLUS,
  RegexTokenType::OPTIONAL,
  RegexTokenType::ALTERNATIVE,
  RegexTokenType::ANY,
  RegexTokenType::ESCAPE,
};

std::string to_string(RegexTokenType t);

struct RegexToken {
  RegexTokenType tokentype;
  // int length;
  int column;
  // char* first;
  std::string_view data;
};

const RegexToken& invalid_token();

#endif // REGEXTOKENS_H_
