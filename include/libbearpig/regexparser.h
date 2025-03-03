#ifndef REGEXPARSER_H_
#define REGEXPARSER_H_

#include <libbearpig/regextokens.h>
#include <optional>
#include <vector>

class RegexParser {
public:
  bool parse();
  bool is_done() { return current_token_idx == tokenstream.size(); };
  explicit RegexParser(std::vector<RegexToken> tokenstream) : tokenstream(tokenstream) {}

private:
  void end_of_input_error();
  void print_error_message_and_exit(const std::string&, int loc);
  bool parse_regex();
  bool parse_quantified_exp();
  bool parse_character();
  bool parse_any();
  bool parse_single_character_token(RegexTokenType type);
  bool parse_elementary_exp();
  bool parse_group();
  std::vector<RegexToken> tokenstream;
  int current_token_idx = 0;
  int backtrack_position = 0;
  bool invalid = false;
  std::optional<RegexToken> get_next_token();
};

#endif // REGEXPARSER_H_
