#include <lib/lib.h>
#include <gtest/gtest.h>

TEST(HELLOWORLD, TESTS_ARE_WORKING){
  EXPECT_EQ(1,1);
}
TEST(HELLOWORLD, TESTS_ARE_NOT_WORKING){
  EXPECT_EQ(1,2);
}
