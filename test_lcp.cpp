#include <iostream>
#include "packed/lcp.hpp"
#include "lcpinstance.hpp"
#include <gtest/gtest.h>

using namespace packed;


constexpr size_t TEST_LENGTH = 100;
void test_lce(size_t (*lce)(const char*, size_t, const char*, size_t)) {
   for(size_t test_length = 0; test_length < TEST_LENGTH; ++test_length) {
      for(size_t test_position = 0; test_position < test_length; ++test_position) {
	 LCPInstance instance(test_length, test_position);
	 ASSERT_EQ(lce(instance.m_stra, instance.m_length, instance.m_strb, instance.m_length), test_position);
      }
   }
}

void test_eq(size_t (*lce)(const char*, size_t, const char*, size_t)) {
   const aligned_string text(2ULL<<13, 'a');
   for(size_t test_length = 1; test_length < text.length(); ++test_length) {
      ASSERT_EQ(lce(text.data(), test_length, text.data(), test_length), test_length);
   }
}

TEST(LCEeq, character) {  test_eq(longest_common_prefix_character); } 
TEST(LCEeq, packed) {  test_eq(longest_common_prefix_packed); }

TEST(LCE, character) {  test_lce(longest_common_prefix_character); } 
TEST(LCE, packed) {  test_lce(longest_common_prefix_packed); }
#ifdef __SSE2__ 
TEST(LCE, sse) {  test_lce(longest_common_prefix_sse); }
TEST(LCEeq, sse) {  test_eq(longest_common_prefix_sse); }
#endif
#ifdef __AVX2__
TEST(LCE, avx2) {  test_lce(longest_common_prefix_avx2); }
TEST(LCEeq, avx2) {  test_eq(longest_common_prefix_avx2); }
#endif
#ifdef __AVX512__
TEST(LCE, avx512) {  test_lce(longest_common_prefix_avx512); }
TEST(LCEeq, avx512) {  test_eq(longest_common_prefix_avx512); }
#endif

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
//   test_eq(longest_common_prefix_packed);
   return RUN_ALL_TESTS();
}
