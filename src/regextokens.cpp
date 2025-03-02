#include <libbearpig/regextokens.h>

std::string to_string(RegexTokenType t) {
  switch (t) {
  case (RegexTokenType::PAREN_OPEN): {
    return "PAREN_OPEN";
  }
  case (RegexTokenType::PAREN_CLOSE): {
    return "PAREN_CLOSE";
  }
  case (RegexTokenType::CURLY_OPEN): {
    return "CURLY_OPEN";
  }
  case (RegexTokenType::CURLY_CLOSE): {
    return "CURLY_CLOSE";
  }
  case (RegexTokenType::SQUARE_OPEN): {
    return "SQUARE_OPEN";
  }
  case (RegexTokenType::SQUARE_CLOSE): {
    return "SQUARE_CLOSE";
  }
  case (RegexTokenType::STAR): {
    return "STAR";
  }
  case (RegexTokenType::PLUS): {
    return "PLUS";
  }
  case (RegexTokenType::ALTERNATIVE): {
    return "ALTERNATIVE";
  }
  case (RegexTokenType::ANY): {
    return "ANY";
  }
  case (RegexTokenType::CHARACTER): {
    return "CHARACTER";
  }
  case (RegexTokenType::ESCAPE): {
    return "ESCAPE";
  }
  case (RegexTokenType::OPTIONAL): {
    return "OPTIONAL";
  }
  case (RegexTokenType::WHITESPACE): {
    return "WHITESPACE";
  }
  default: {
    return "INVALID";
  }
  }
}
