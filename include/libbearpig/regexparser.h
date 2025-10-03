#ifndef REGEXPARSER_H_
#define REGEXPARSER_H_

#include <libbearpig/regextokens.h>
#include <optional>
#include <vector>

class RegexParser {
public:
  bool parse();
  bool is_done() { return current_token_idx == tokenstream.size(); };
  int get_current_token_idx() const {
    return current_token_idx;
  }
  int get_size_of_tokenstream() const {
    return tokenstream.size();
  }
  explicit RegexParser(std::vector<RegexToken> tokenstream) : tokenstream(tokenstream) {}

private:
  void end_of_input_error();
  void print_error_message_and_exit(const std::string&, int loc);
  bool parse_regex();
  bool parse_simple_exp();
  bool parse_concatenation_exp();
  bool parse_quantified_exp();
  bool parse_character();
  bool parse_any();
  bool parse_single_character_token(RegexTokenType type);
  // XXX (jlier):
  // create version of this with an array of RegexTokenTypes, so we can
  // match the next character with one of several types
  // useful for escaping metacharacters
  // bool parse_single_character_token(std::array<RegexTokenType> type);
  bool parse_elementary_exp();
  bool parse_group();
  bool parse_set();
  bool parse_set_items();
  std::vector<RegexToken> tokenstream;
  int current_token_idx = 0;
  int backtrack_position = 0;
  bool invalid = false;
  std::optional<RegexToken> get_next_token();
  std::optional<RegexToken> peek();
  bool expect(std::optional<RegexToken> actual, RegexTokenType expected);
};

#endif // REGEXPARSER_H_
