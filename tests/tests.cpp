#include "libbearpig/regexast.h"
#include "libbearpig/regexparser.h"
#include "libbearpig/regexscanner.h"
#include "libbearpig/regextokens.h"
#include <gtest/gtest.h>
#include <libbearpig/lib.h>

TEST(REGEXPARSER, basic_able_to_parse_characters) {
  RegexScanner rs{"a"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());

  auto top = rp.get_top_of_expression();

  auto &alternatives = top->alternatives;
  EXPECT_EQ(alternatives.size(), 1);

  auto &quantified_exps = alternatives.front().exps;
  EXPECT_EQ(quantified_exps.size(), 1);

  auto &quantified_exp = quantified_exps.front();

  EXPECT_EQ(quantified_exp.quantifier, QuantifiedExp::Quantifier::NONE);

  auto rchar = ((RChar *)quantified_exp.exp.get());
  EXPECT_EQ(rchar->character.data, 'a');
  EXPECT_EQ(rchar->character.column, 0);
  EXPECT_EQ(rchar->character.tokentype, RegexTokenType::CHARACTER);
}

TEST(REGEXPARSER, basic_able_to_parse_groups) {
  RegexScanner rs{"((a))"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_sets) {
  RegexScanner rs{"[az]"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_sets_with_ranges) {
  RegexScanner rs{"[a-d]"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_sets_with_multiple_ranges) {
  RegexScanner rs{"[a-df0-9]"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_negative_sets_with_multiple_ranges) {
  RegexScanner rs{"[^a-df0-9]"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_any) {
  RegexScanner rs{"."};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_nested_group_with_any) {
  RegexScanner rs{"((.))"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_plus) {
  RegexScanner rs{"(a+)+"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_star) {
  RegexScanner rs{"(a*)*"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_optional) {
  RegexScanner rs{"a?"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_concat) {
  RegexScanner rs{"aaaaabaab."};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_a_bunch_of_stuff) {
  RegexScanner rs{"a.+(a?b*c+)*f"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_alternative) {
  RegexScanner rs{"a|b"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_escape_sequence) {
  RegexScanner rs{R"(a\\)"};

  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
