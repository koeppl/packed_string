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

void test64_lcp(const lcp64_function_type& lcp) {
   for(size_t test_length = 0; test_length < TEST_LENGTH/sizeof(uint64_t); ++test_length) {
      for(size_t test_position = 0; test_position < test_length; ++test_position) {
	 LCPInstance instance(test_length*sizeof(uint64_t), test_position);
	 ASSERT_EQ(lcp(reinterpret_cast<const uint64_t*>(instance.m_stra), reinterpret_cast<const uint64_t*>(instance.m_strb), instance.m_length/sizeof(uint64_t)), test_position/sizeof(uint64_t));
      }
   }
}

void test64_eq(const lcp64_function_type& lcp) {
   const aligned_string text(2ULL<<13, 'a');
   for(size_t test_length = 1; test_length < text.length()/sizeof(uint64_t); ++test_length) {
      ASSERT_EQ(lcp(reinterpret_cast<const uint64_t*>(text.data()), reinterpret_cast<const uint64_t*>(text.data()), test_length), test_length);
   }
}

void test64_lcp_offset(const lcp64_function_type& lcp) {
   for(size_t test_length = 0; test_length < TEST_LENGTH/sizeof(uint64_t); ++test_length) {
      for(size_t test_position = 0; test_position < test_length; ++test_position) {
	 LCPInstance instance(test_length*sizeof(uint64_t), test_position);
	 const uint64_t* a = reinterpret_cast<const uint64_t*>(instance.m_stra);
	 const uint64_t* b = reinterpret_cast<const uint64_t*>(instance.m_strb);
	 for(size_t offset_length = 0; offset_length <= test_position/sizeof(uint64_t); ++offset_length) {
	    const size_t ret = longest_common_prefix_offset(a, offset_length, b, offset_length, test_length-offset_length, lcp);
	    ASSERT_EQ(ret, test_position/sizeof(uint64_t)-offset_length);
	 }
	 // test offset only for one side
	 for(size_t offset_length = 0; offset_length <= test_position/sizeof(uint64_t); ++offset_length) {
	    const size_t ret = longest_common_prefix_offset(a, offset_length, b, 0, test_length-offset_length, lcp);
	    ASSERT_EQ(ret, test_position/sizeof(uint64_t)-offset_length);
	 }
	 // test offset only for the other side
	 for(size_t offset_length = 0; offset_length <= test_position/sizeof(uint64_t); ++offset_length) {
	    const size_t ret = longest_common_prefix_offset(a, 0, b, offset_length, test_length-offset_length, lcp);
	    ASSERT_EQ(ret, test_position/sizeof(uint64_t)-offset_length);
	 }
      }
   }
}

void test64_eq_offset(const lcp64_function_type& lcp) {
   const aligned_string text(2ULL<<10, 'a');
   for(size_t test_length = 1; test_length < text.length()/sizeof(uint64_t); ++test_length) {
      for(size_t offset_length = 0; offset_length <= test_length; ++offset_length) {
	 const uint64_t* a = reinterpret_cast<const uint64_t*>(text.data());
	 const size_t ret = longest_common_prefix_offset(a, offset_length, a, offset_length, test_length-offset_length, lcp);
	 ASSERT_EQ(ret, test_length-offset_length);
      }
   }
}


TEST(LCP64eq_off, character) {  test64_eq_offset(longest_common_prefix_character); } 
TEST(LCP64_off, character) {  test64_lcp_offset(longest_common_prefix_character); } 
TEST(LCP64eq, character) {  test64_eq(longest_common_prefix_character); } 
TEST(LCP64, character) {  test64_lcp(longest_common_prefix_character); } 

TEST(LCPeq, character) {  test_eq(longest_common_prefix_character); } 
TEST(LCPeq, packed) {  test_eq(longest_common_prefix_packed); }

TEST(LCP, character) {  test_lcp(longest_common_prefix_character); } 
TEST(LCP, packed) {  test_lcp(longest_common_prefix_packed); }

#ifdef __SSE2__ 
TEST(LCP, sse) {  test_lcp(longest_common_prefix_sse); }
TEST(LCPeq, sse) {  test_eq(longest_common_prefix_sse); }
#endif

#ifdef __SSE3__ 
TEST(LCP64, sse) {  test64_lcp(longest_common_prefix_sse); }
TEST(LCP64eq, sse) {  test64_eq(longest_common_prefix_sse); }
TEST(LCP64eq_off, sse) {  test64_eq_offset(longest_common_prefix_sse); }
TEST(LCP64_off, sse) {  test64_lcp_offset(longest_common_prefix_sse); }
#endif

#ifdef __AVX2__
TEST(LCP, avx2_8) {  test_lcp(longest_common_prefix_avx2_8); }
TEST(LCPeq, avx2_8) {  test_eq(longest_common_prefix_avx2_8); }

TEST(LCP, avx2) {  test_lcp(longest_common_prefix_avx2); }
TEST(LCPeq, avx2) {  test_eq(longest_common_prefix_avx2); }

TEST(LCP64, avx2) {  test64_lcp(longest_common_prefix_avx2); }
TEST(LCP64eq, avx2) {  test64_eq(longest_common_prefix_avx2); }
TEST(LCP64eq_off, avx2) {  test64_eq_offset(longest_common_prefix_avx2); }
TEST(LCP64_off, avx2) {  test64_lcp_offset(longest_common_prefix_avx2); }
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
