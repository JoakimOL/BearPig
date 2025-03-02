#ifndef REGEXTOKENS_H_
#define REGEXTOKENS_H_

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
  STAR,
  PLUS,

  // Choice
  ALTERNATIVE,
  OPTIONAL,

  // Characters
  ANY,
  WHITESPACE,
  CHARACTER,
  ESCAPE,

  // maybe a good idea?
  INVALID,
};

std::string to_string(RegexTokenType t);

struct RegexToken {
  RegexTokenType tokentype;
  // int length;
  int column;
  // char* first;
  std::string_view data;
};

#endif // REGEXTOKENS_H_
