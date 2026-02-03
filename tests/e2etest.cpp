#include "libbearpig/nfa.h"
#include "libbearpig/nfagenvisitor.h"
#include "libbearpig/regexast.h"
#include "libbearpig/regexparser.h"
#include "libbearpig/regexscanner.h"
#include <gtest/gtest.h>
#include <libbearpig/lib.h>

using namespace bp;

void setup(std::string_view regex, NFA &nfa) {
  RegexScanner rs{regex};

  auto tokens = rs.tokenize();
  RegexParser rp{tokens};
  rp.parse();
  EXPECT_TRUE(rp.is_done());

  auto *top = rp.get_top_of_expression();
  EXPECT_NE(top, nullptr);

  NfaGenVisitor nfa_generator{nfa, tokens};
  top->apply(&nfa_generator);
}

TEST(E2E, Basic_E2E_test) {
  const std::string input{"a"};
  NFA nfa;
  setup("a", nfa);

  auto match = nfa.exact_match(input);
  EXPECT_TRUE(match.success);
  EXPECT_EQ(match.match, "a");
  EXPECT_EQ(match.length, 1);
}

TEST(E2E, That_example_that_broke_find_all_matches) {
  const std::string input{"aaaaaabcbcbabcbcbacbCBACBCBacbcbacb09090abCBab09cb0a)0"};
  NFA nfa;
  setup("([a-zA-Z]+|[0-9][0-9]?)+", nfa);

  auto match = nfa.exact_match(input);
  EXPECT_FALSE(match.success);
  EXPECT_EQ(match.match, "");
  EXPECT_EQ(match.length, 0);

  auto matches = nfa.find_all_matches(input);
  EXPECT_EQ(matches.size(), 2);
  {
    match = matches[0];
    EXPECT_TRUE(match.success);
    EXPECT_EQ(match.match, "aaaaaabcbcbabcbcbacbCBACBCBacbcbacb09090abCBab09cb0a");
    EXPECT_EQ(match.length, 52);
  }
  {
    match = matches[1];
    EXPECT_TRUE(match.success);
    EXPECT_EQ(match.match, "0");
    EXPECT_EQ(match.length, 1);
  }
}
