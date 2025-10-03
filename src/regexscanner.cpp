#include <libbearpig/regexscanner.h>

#include <spdlog/spdlog.h>

RegexScanner::RegexScanner(const std::string_view input)
    : input{input}, current_column{0} {
  spdlog::info("{}::initialized with input={}", __func__, input);
}

std::vector<RegexToken> RegexScanner::tokenize() {
  std::vector<RegexToken> tokens;
  if (input.empty()) {
    return tokens;
  }
  spdlog::info(input.size());
  while (current_column < input.size()) {
    tokens.emplace_back(next());
  }
  for (auto token : tokens) {
    spdlog::info("token: {} ({}) at {}", token.data,
                 to_string(token.tokentype), token.column);
  }
  return tokens;
}

RegexToken RegexScanner::next() {
  char current_char = input.at(current_column);
  RegexToken token;
  spdlog::info("current_char={}, column = {}", current_char, current_column);
  switch (current_char) {
  case ('*'): {
    token = RegexToken{RegexTokenType::STAR, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('('): {
    token = RegexToken{RegexTokenType::PAREN_OPEN, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('['): {
    token = RegexToken{RegexTokenType::SQUARE_OPEN, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case (')'): {
    token = RegexToken{RegexTokenType::PAREN_CLOSE, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case (']'): {
    token = RegexToken{RegexTokenType::SQUARE_CLOSE, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('?'): {
    token = RegexToken{RegexTokenType::OPTIONAL, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('.'): {
    token = RegexToken{RegexTokenType::ANY, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('-'): {
    token = RegexToken{RegexTokenType::DASH, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('^'): {
    token = RegexToken{RegexTokenType::CARET, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('|'): {
    token = RegexToken{RegexTokenType::ALTERNATIVE, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('+'): {
    token = RegexToken{RegexTokenType::PLUS, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('\\'): {
    token = RegexToken{RegexTokenType::ESCAPE, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  case ('\n'): // do we need to handle newlines in particular?
  case (' '): {
    token = RegexToken{RegexTokenType::WHITESPACE, current_column,
                       input.substr(current_column, 1)};
    break;
  }
  default: {
    token = RegexToken{RegexTokenType::CHARACTER, current_column,
                       input.substr(current_column, 1)};
  }
  }
  current_column++;
  return token;
}
