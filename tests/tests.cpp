#include "libbearpig/regexparser.h"
#include "libbearpig/regexscanner.h"
#include <libbearpig/lib.h>
#include <gtest/gtest.h>

TEST(REGEXPARSER, basic_able_to_parse_characters){
  RegexScanner rs{"a"};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_groups){
  RegexScanner rs{"((a))"};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_any){
  RegexScanner rs{"."};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_nested_group_with_any){
  RegexScanner rs{"((.))"};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
TEST(REGEXPARSER, basic_able_to_parse_plus){
  RegexScanner rs{"(a+)+"};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_star){
  RegexScanner rs{"(a*)*"};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}

TEST(REGEXPARSER, basic_able_to_parse_optional){
  RegexScanner rs{"a?"};
  
  RegexParser rp{rs.tokenize()};
  EXPECT_TRUE(rs.is_at_end());
  EXPECT_TRUE(rp.parse());
  EXPECT_TRUE(rp.is_done());
}
