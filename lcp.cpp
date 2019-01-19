#include "packed/lcp.hpp"
#include "packed/character.hpp"
#include <immintrin.h>

namespace packed {

size_t longest_common_prefix_character(const char* a, const char* b, const size_t length) {
   for(size_t i = 0; i < length; ++i) {
      if(a[i] != b[i]) return i;
   }
   return length;
}

size_t longest_common_prefix_packed(const char* a, const char* b, const size_t length) {
   const size_t packed_length = length/character::FIT_CHARS;
   for(size_t i = 0; i < packed_length; ++i) {
      const size_t pos = i*character::FIT_CHARS;
      const uint64_t packed_a = character::construct(a,pos);
      const uint64_t packed_b = character::construct(b,pos);
      if(packed_a != packed_b) {
	 DCHECK_EQ(character::longest_common_prefix(packed_a, packed_b)+pos, 
	       longest_common_prefix_character(a,a_length,b,b_length));
	 return character::longest_common_prefix(packed_a, packed_b)+pos;
      }
   }
   const size_t compared_chars = packed_length*character::FIT_CHARS;
   if(compared_chars == length) {
      DCHECK_EQ(length, longest_common_prefix_character(a,b,length));
      return length;
   }
   const uint64_t packed_a = character::construct(a, compared_chars, length - compared_chars);
   const uint64_t packed_b = character::construct(b, compared_chars, length - compared_chars);
   DCHECK_EQ(character::char_length(packed_a), a_length - compared_chars);
   DCHECK_EQ(character::char_length(packed_b), b_length - compared_chars);

   DCHECK_EQ(character::longest_common_prefix(packed_a, packed_b), 
	 longest_common_prefix_character(a+compared_chars,a_length - compared_chars,  b+compared_chars, b_length - compared_chars));

   const size_t ret = compared_chars + character::longest_common_prefix(packed_a, packed_b);
   DCHECK_EQ(ret, longest_common_prefix_character(a,a_length,b,b_length));
   return ret;
}

#ifdef __SSE2__
size_t longest_common_prefix_sse(const char*const a, const char*const b, const size_t length) {
   constexpr size_t register_size = 128/8;
   size_t c_length = 0; // length in register_size units
   for(; c_length < length / register_size; ++c_length) {
      __m128i ma = _mm_load_si128((__m128i*)(a+c_length*register_size)); 
      __m128i mb = _mm_load_si128((__m128i*)(b+c_length*register_size)); 
      const int16_t mask = _mm_movemask_epi8(_mm_cmpeq_epi8(ma,mb));

      if(~mask == 0) {
	 continue;
      }
      if(mask == 0) {
	 return c_length == 0 ?  0 : (c_length-1)*register_size;
      }
      const size_t pos = __builtin_ctz(~mask);
      const size_t ret = c_length*register_size + pos;
      DCHECK_EQ(ret, longest_common_prefix_character(a, a_length, b, b_length));
      return ret;
   }
   const size_t ret = c_length*register_size + 
      longest_common_prefix_packed(
	    a + c_length*register_size,
	    b + c_length*register_size, 
	    length - c_length*register_size);
   DCHECK_EQ(ret, longest_common_prefix_character(a, a_length, b, b_length));
   return ret;
}
#endif //SSE2

#ifdef __AVX2__
size_t longest_common_prefix_avx2(const char*const a, const char*const b, const size_t length) {
   constexpr size_t register_size = 256/8;
   size_t c_length = 0; // c_length in register_size units
   for(; c_length < length / register_size; ++c_length) {
      __m256i ma = _mm256_load_si256((__m256i*)(a+c_length*register_size)); 
      __m256i mb = _mm256_load_si256((__m256i*)(b+c_length*register_size)); 
      const unsigned int mask = _mm256_movemask_epi8(_mm256_cmpeq_epi64 (ma, mb));
      if(~mask == 0) {
	 continue;
      }
      if(mask == 0) {
	 return c_length == 0 ?  0 : (c_length-1)*register_size;
      }
      const size_t least_significant = __builtin_ctz(~mask);
      DCHECK_EQ(((least_significant)/character::FIT_CHARS)*character::FIT_CHARS, least_significant);
      const size_t pos = (least_significant)/character::FIT_CHARS;
      const uint64_t packed_a = character::construct(a+c_length*register_size,(pos)*character::FIT_CHARS);
      const uint64_t packed_b = character::construct(b+c_length*register_size,(pos)*character::FIT_CHARS);
      const size_t ret = c_length*register_size + character::longest_common_prefix(packed_a, packed_b)+pos*character::FIT_CHARS;
      DCHECK_EQ(ret, longest_common_prefix_character(a, a_length, b, b_length));
      return ret;
   }
   const size_t ret = c_length*register_size + 
      longest_common_prefix_packed(
	    a + c_length*register_size,
	    b + c_length*register_size, 
	    length - c_length*register_size);
   DCHECK_EQ(ret, longest_common_prefix_character(a, a_length, b, b_length));
   return ret;
}
#endif

#ifdef __AVX512__

#pragma message ("Warning: the code for AVX512 has not been tested.") 
// Sadly, I do not have a computer with AVX512 instructions

size_t longest_common_prefix_avx512(const char*const a, const char*const b, const size_t length) {
   constexpr size_t register_size = 512/8;
   size_t c_length = 0; // c_length in register_size units
   for(; c_length < length / register_size; ++c_length) {

   __m512i ma = _mm512_load_si512((__m128i*)(a+c_length*register_size)); 
   __m512i mb = _mm512_load_si512((__m128i*)(b+c_length*register_size)); 
   const unsigned int mask = _mm512_cmpeq_epu64_mask (ma, mb);
      if(~mask == 0) {
	 continue;
      }
      if(mask == 0) {
	 return c_length == 0 ?  0 : (c_length-1)*register_size;
      }
      const size_t least_significant = __builtin_ctz(~mask);
      DCHECK_EQ(((least_significant)/character::FIT_CHARS)*character::FIT_CHARS, least_significant);
      const size_t pos = (least_significant)/character::FIT_CHARS;
      const uint64_t packed_a = character::construct(a+c_length*register_size,(pos)*character::FIT_CHARS);
      const uint64_t packed_b = character::construct(b+c_length*register_size,(pos)*character::FIT_CHARS);
      const size_t ret = c_length*register_size + character::longest_common_prefix(packed_a, packed_b)+pos*character::FIT_CHARS;
      DCHECK_EQ(ret, longest_common_prefix_character(a, a_length, b, b_length));
      return ret;
   }
   const size_t ret = c_length*register_size + 
      longest_common_prefix_packed(
	    a + c_length*register_size,
	    b + c_length*register_size, 
	    length - c_length*register_size);
   DCHECK_EQ(ret, longest_common_prefix_character(a, a_length, b, b_length));
   return ret;
}
#endif//avx512

}//ns
