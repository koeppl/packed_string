#include <iostream>
#include "packed/lcp.hpp"
#include "lcpinstance.hpp"
#include <gtest/gtest.h>

using namespace packed;


constexpr size_t TEST_LENGTH = 100;
void test_lcp(const lcp_function_type& lcp) {
   for(size_t test_length = 0; test_length < TEST_LENGTH; ++test_length) {
      for(size_t test_position = 0; test_position < test_length; ++test_position) {
	 LCPInstance instance(test_length, test_position);
	 ASSERT_EQ(lcp(instance.m_stra, instance.m_strb, instance.m_length), test_position);
      }
   }
}

void test_eq(const lcp_function_type& lcp) {
   const aligned_string text(2ULL<<13, 'a');
   for(size_t test_length = 1; test_length < text.length(); ++test_length) {
      ASSERT_EQ(lcp(text.data(), text.data(), test_length), test_length);
   }
}

TEST(LCPeq, character) {  test_eq(longest_common_prefix_character); } 
TEST(LCPeq, packed) {  test_eq(longest_common_prefix_packed); }

TEST(LCP, character) {  test_lcp(longest_common_prefix_character); } 
TEST(LCP, packed) {  test_lcp(longest_common_prefix_packed); }
#ifdef __SSE2__ 
TEST(LCP, sse) {  test_lcp(longest_common_prefix_sse); }
TEST(LCPeq, sse) {  test_eq(longest_common_prefix_sse); }
#endif
#ifdef __AVX2__
TEST(LCP, avx2) {  test_lcp(longest_common_prefix_avx2); }
TEST(LCPeq, avx2) {  test_eq(longest_common_prefix_avx2); }
#endif
#ifdef __AVX512__
TEST(LCP, avx512) {  test_lcp(longest_common_prefix_avx512); }
TEST(LCPeq, avx512) {  test_eq(longest_common_prefix_avx512); }
#endif

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
//   test_eq(longest_common_prefix_packed);
   return RUN_ALL_TESTS();
}
