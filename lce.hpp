#include <iostream>
#include "packed_string.hpp"
#include <immintrin.h>

size_t longest_common_prefix_naive(const char* a, const char* b) {
   size_t i = 0;
   for(; a[i] != 0 && b[i] != 0; ++i) {
      if(a[i] != b[i]) return i;
   }
   return i == 0 ? 0 : i-1;
}

size_t longest_common_prefix_packed(const char* a, const size_t a_length, const char* b, const size_t b_length) {
   const size_t min_length = std::min(a_length, b_length)/packed_character::FIT_CHARS;
   for(size_t i = 0; i < min_length; ++i) {
      const size_t pos = i*packed_character::FIT_CHARS;
      const uint64_t packed_a = packed_character::construct(a,pos);
      const uint64_t packed_b = packed_character::construct(b,pos);
      if(packed_a != packed_b) {
	 DCHECK_EQ(packed_character::longest_common_prefix(packed_a, packed_b)+pos, longest_common_prefix_naive(a,b));
	 return packed_character::longest_common_prefix(packed_a, packed_b)+pos;
      }
   }
   const size_t compared_chars = min_length*packed_character::FIT_CHARS;
   const uint64_t packed_a = packed_character::construct(a,compared_chars,a_length - compared_chars);
   const uint64_t packed_b = packed_character::construct(b,compared_chars,b_length - compared_chars);

   DCHECK_EQ(packed_character::longest_common_prefix(packed_a, packed_b), longest_common_prefix_naive(a+min_length*packed_character::FIT_CHARS, b+min_length*packed_character::FIT_CHARS));

   const size_t ret = min_length*packed_character::FIT_CHARS + packed_character::longest_common_prefix(packed_a, packed_b);
   DCHECK_EQ(ret, longest_common_prefix_naive(a,b));
   return ret;
}

#ifdef __SSE2__
size_t longest_common_prefix_sse(const char*const a, const size_t a_length, const char*const b, const size_t b_length) {
   const size_t min_length = std::min(a_length, b_length);
   constexpr size_t register_size = 128/8;
   size_t length = 0; // length in register_size units
   for(; length < min_length / register_size; ++length) {
      __m128i ma = _mm_load_si128((__m128i*)(a+length*register_size)); 
      __m128i mb = _mm_load_si128((__m128i*)(b+length*register_size)); 
      const int16_t mask = _mm_movemask_epi8(_mm_cmpeq_epi8(ma,mb));

      if(~mask == 0) {
	 continue;
      }
      if(mask == 0) {
	 return length == 0 ?  0 : (length-1)*register_size;
      }
      const size_t pos = __builtin_ctz(~mask);
      const size_t ret = length*register_size + pos;
      DCHECK_EQ(ret, longest_common_prefix_naive(a, b));
      return ret;
   }
   const size_t ret = length*register_size + 
      longest_common_prefix_packed(
	    a + length*register_size,
	    a_length - length*register_size, 
	    b + length*register_size, 
	    b_length - length*register_size);
   DCHECK_EQ(ret, longest_common_prefix_naive(a, b));
   return ret;
}
#endif //SSE2

#ifdef __AVX2__
size_t longest_common_prefix_avx2(const char*const a, const size_t a_length, const char*const b, const size_t b_length) {
   const size_t min_length = std::min(a_length, b_length);
   constexpr size_t register_size = 256/8;
   size_t length = 0; // length in register_size units
   for(; length < min_length / register_size; ++length) {
      __m256i ma = _mm256_load_si256((__m256i*)(a+length*register_size)); 
      __m256i mb = _mm256_load_si256((__m256i*)(b+length*register_size)); 
      const unsigned int mask = _mm256_movemask_epi8(_mm256_cmpeq_epi64 (ma, mb));
      if(~mask == 0) {
	 continue;
      }
      if(mask == 0) {
	 return length == 0 ?  0 : (length-1)*register_size;
      }
      const size_t least_significant = __builtin_ctz(~mask);
      DCHECK_EQ(((least_significant)/packed_character::FIT_CHARS)*packed_character::FIT_CHARS, least_significant);
      const size_t pos = (least_significant)/packed_character::FIT_CHARS;
      const uint64_t packed_a = packed_character::construct(a+length*register_size,(pos)*packed_character::FIT_CHARS);
      const uint64_t packed_b = packed_character::construct(b+length*register_size,(pos)*packed_character::FIT_CHARS);
      const size_t ret = length*register_size + packed_character::longest_common_prefix(packed_a, packed_b)+pos*packed_character::FIT_CHARS;
      DCHECK_EQ(ret, longest_common_prefix_naive(a, b));
      return ret;
   }
   const size_t ret = length*register_size + 
      longest_common_prefix_packed(
	    a + length*register_size,
	    a_length - length*register_size, 
	    b + length*register_size, 
	    b_length - length*register_size);
   DCHECK_EQ(ret, longest_common_prefix_naive(a, b));
   return ret;
}
#endif

#ifdef __AVX512__
size_t longest_common_prefix_avx512(const char*const a, const size_t a_length, const char*const b, const size_t b_length) {
   const size_t min_length = std::min(a_length, b_length);
   constexpr size_t register_size = 512/8;
   size_t length = 0; // length in register_size units
   for(; length < min_length / register_size; ++length) {

   __m512i ma = _mm512_load_si512((__m128i*)(a+length*register_size)); 
   __m512i mb = _mm512_load_si512((__m128i*)(b+length*register_size)); 
   const unsigned int mask = _mm512_cmpeq_epu64_mask (ma, mb);
      if(~mask == 0) {
	 continue;
      }
      if(mask == 0) {
	 return length == 0 ?  0 : (length-1)*register_size;
      }
      const size_t least_significant = __builtin_ctz(~mask);
      DCHECK_EQ(((least_significant)/packed_character::FIT_CHARS)*packed_character::FIT_CHARS, least_significant);
      const size_t pos = (least_significant)/packed_character::FIT_CHARS;
      const uint64_t packed_a = packed_character::construct(a+length*register_size,(pos)*packed_character::FIT_CHARS);
      const uint64_t packed_b = packed_character::construct(b+length*register_size,(pos)*packed_character::FIT_CHARS);
      const size_t ret = length*register_size + packed_character::longest_common_prefix(packed_a, packed_b)+pos*packed_character::FIT_CHARS;
      DCHECK_EQ(ret, longest_common_prefix_naive(a, b));
      return ret;
   }
   const size_t ret = length*register_size + 
      longest_common_prefix_packed(
	    a + length*register_size,
	    a_length - length*register_size, 
	    b + length*register_size, 
	    b_length - length*register_size);
   DCHECK_EQ(ret, longest_common_prefix_naive(a, b));
   return ret;
}
#endif//avx512

#include <memory>


template<typename T, int alignment = 32>
class aligned_allocator : public std::allocator <T> {
public:
     typedef size_t     size_type;
     typedef ptrdiff_t  difference_type;
     typedef T*         pointer;
     typedef const T*   const_pointer;
     typedef T&         reference;
     typedef const T&   const_reference;
     typedef T          value_type;


     template<typename U>
     struct rebind
     {
       typedef aligned_allocator <U> other; 
     };

     aligned_allocator() {}

     template<typename U>
     aligned_allocator(const aligned_allocator<U>&) {}

     pointer allocate(size_type n, std::allocator<void>::const_pointer  = 0) {
	return static_cast<pointer>(aligned_alloc(alignment, n*sizeof(T)));
     }
};




//
// int main() {
//    using string_type = std::basic_string<char, std::char_traits<char>, aligned_allocator<char>>;
//    std::basic_string<char, std::char_traits<char>, aligned_allocator<char>> a = "aaaafa";
//    std::basic_string<char, std::char_traits<char>, aligned_allocator<char>> b = "aaaafbb";
//    longest_common_prefix_sse(a.data(), a.length(), b.data(), b.length());
//    // longest_common_prefix_avx512(a.data(), a.length(), b.data(), b.length());
//    //
//    //
//    
//       // std::cout << _mm_cmpistrc(ma, mb,  _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_LEAST_SIGNIFICANT) << std::endl;
//       // const unsigned int mask = _mm128_movemask_epi8(_mm128_cmpeq_epi64 (ma, mb));
// }


