#ifndef REGEXPARSER_H_
#define REGEXPARSER_H_

#include <libbearpig/regextokens.h>
#include <libbearpig/regexast.h>
#include <memory>
#include <vector>

class RegexParser {
public:
  bool parse();
  bool is_done() { return current_token_idx == tokenstream.size(); };
  int get_current_token_idx() const { return current_token_idx; }
  int get_size_of_tokenstream() const { return tokenstream.size(); }
  AlternativeExp* get_top_of_expression() const { return expression_top.get(); }
  RegexParser() = delete;
  explicit RegexParser(std::vector<RegexToken> tokens)
      : tokenstream(tokens), current_token_idx{0},
        current_token{tokenstream[current_token_idx]} {
  }

private:
  bool parse_top_level();
  AlternativeExp parse_exp();
  ConcatExp parse_simple_exp();
  ConcatExp parse_concatenation_exp();
  QuantifiedExp parse_quantified_exp();
  AlternativeExp parse_alternative();
  std::unique_ptr<ElementaryExp> parse_elementary_exp();
  AnyExp parse_any();
  GroupExp parse_group();
  SetExp parse_set();
  std::vector<SetItem> parse_set_items();
  SetItem parse_set_item();
  RChar parse_character(bool single = false);
  EscapeSeq parse_escape_seq();

  void end_of_input_error();
  template <typename... Rs>
  void unexpected_token_error(std::string_view func, Rs &&...expected);
  void unexpected_token_error(std::string_view func,
                              const RegexTokenType &expected);
  void advance();
  void consume(RegexTokenType expected);
  void consume(std::string_view func, RegexTokenType expected);
  void print_error_message_and_exit(const std::string &, int loc);
  std::vector<RegexToken> tokenstream;
  std::unique_ptr<AlternativeExp> expression_top;
  int current_token_idx;
  RegexToken current_token;
  bool invalid = false;
};

#endif // REGEXPARSER_H_
