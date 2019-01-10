#include <iostream>
#include "lce.hpp"
#include "lceinstance.hpp"
#include <gtest/gtest.h>


constexpr size_t TEST_LENGTH = 100;
void test_lce(size_t (*lce)(const char*, size_t, const char*, size_t)) {
   for(size_t test_length = 0; test_length < TEST_LENGTH; ++test_length) {
      for(size_t test_position = 0; test_position < test_length; ++test_position) {
	 LCEInstance instance(test_length, test_position);
	 ASSERT_EQ(lce(instance.m_stra, instance.m_length, instance.m_strb, instance.m_length), test_position);
      }
   }
}


TEST(LCE, character) {  test_lce(longest_common_prefix_character); } 
TEST(LCE, packed) {  test_lce(longest_common_prefix_packed); }
#ifdef __SSE2__ 
TEST(LCE, sse) {  test_lce(longest_common_prefix_sse); }
#endif
#ifdef __AVX2__
TEST(LCE, avx2) {  test_lce(longest_common_prefix_avx2); }
#endif

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
