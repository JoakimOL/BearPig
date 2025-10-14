#ifndef REGEXPARSER_H_
#define REGEXPARSER_H_

#include <libbearpig/regextokens.h>
#include <span>
#include <vector>

struct ElementaryExp {};

struct RChar : ElementaryExp {
  int start;
  int end;
  std::span<RegexToken> characters;
};

struct QuantifiedExp {
  enum class Quantifier {
    STAR,
    PLUS,
    OPTIONAL,
  };
  Quantifier quantifier;
  ElementaryExp exp;
};

struct ConcatExp {
  std::vector<QuantifiedExp> exps{};
  void merge(const ConcatExp& other){
    for(QuantifiedExp e: other.exps){
      exps.emplace_back(e);
    }
  }
};

struct AlternativeExp {
  std::vector<ConcatExp> alternatives;
};

class RegexParser {
public:
  bool parse();
  bool is_done() { return current_token_idx == tokenstream.size(); };
  int get_current_token_idx() const { return current_token_idx; }
  int get_size_of_tokenstream() const { return tokenstream.size(); }
  AlternativeExp get_top_of_expression() const { return expression_top; }
  RegexParser() = delete;
  explicit RegexParser(std::vector<RegexToken> tokens)
      : tokenstream(tokens), current_token_idx{0},
        current_token{tokenstream[current_token_idx]} {
    // for (auto token : tokenstream) {
    //   spdlog::info("{}::token: {} ({}) at {}", __func__, token.data,
    //                to_string(token.tokentype), token.column);
    // }
  }

private:
  bool parse_top_level();
  AlternativeExp parse_exp();
  ConcatExp parse_simple_exp();
  ConcatExp parse_concatenation_exp();
  QuantifiedExp parse_quantified_exp();
  ElementaryExp parse_elementary_exp();
  AlternativeExp parse_alternative();
  RChar parse_character();
  bool parse_any();
  bool parse_group();
  bool parse_set();
  bool parse_set_items();
  bool parse_set_item();
  // bool parse_top_level();

  void end_of_input_error();
  void unexpected_token_error(std::string_view func,
                              const RegexTokenType &expected);
  void advance();
  const RegexToken &peek();
  void consume(RegexTokenType expected);
  void consume(std::string_view func, RegexTokenType expected);
  void print_error_message_and_exit(const std::string &, int loc);
  std::vector<RegexToken> tokenstream;
  AlternativeExp expression_top;
  int current_token_idx;
  RegexToken current_token;
  bool invalid = false;
};

#endif // REGEXPARSER_H_
