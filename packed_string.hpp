#pragma once

#include "dcheck.hpp"
#include <climits>
#include <cstdint>
#include <string>

#ifdef NDEBUG
DCHECK(static_assert(false)) // do not run DCHECKS if NDEBUG is on
#define __constexpr constexpr
#else
#define __constexpr inline
#endif


namespace math {

//! computes ceil(dividend/divisor)
template<class T>
constexpr T ceil_div(const T& dividend, const T& divisor) { 
   return (dividend + divisor - 1) / divisor;
}

constexpr int most_significant_bit(const uint64_t& x) {
    return x == 0 ? -1 : (sizeof(uint64_t)*8-1) - __builtin_clzll(x);
}

// largest power of two fits into interval (a,b]
constexpr uint_fast8_t two_fattest_number(const uint64_t &a, const uint64_t &b) {
   return a == b ? 0 : ((-1ULL << most_significant_bit(a ^ b)) & b);
}
}//ns

namespace packed_character {

static constexpr std::size_t FIT_CHARS = sizeof(uint64_t)/sizeof(char);

//! number of characters stored in packed_character a
constexpr uint_fast8_t char_length(const uint64_t &a) {
   return a == 0 ? 0 : (math::most_significant_bit(a)/CHAR_BIT + 1);
}

//! longest common prefix of two packed characters
constexpr uint_fast8_t longest_common_prefix(const uint64_t &a, const uint64_t &b) {
   return (a == b ?  char_length(a) : (__builtin_ctzll(a ^ b) / CHAR_BIT ));
}

//! retrieve character stored in packed_character
__constexpr char character(const uint64_t& packed_character, const std::size_t& index) {
   DCHECK_LT(index, sizeof(uint64_t)/sizeof(char));
   return (packed_character >> (index * CHAR_BIT)) & UCHAR_MAX;
}

__constexpr uint64_t substring(const uint64_t &packed_character, const uint_fast8_t& from, const uint_fast8_t& length) {
   DCHECK_GT(length,0);
   DCHECK_LE(from+length, char_length(packed_character));
   return (packed_character >> (from * CHAR_BIT)) & ((-1ULL)>>(64-length*CHAR_BIT));
}

constexpr uint64_t truncate(const uint64_t& packed_character, const uint_fast8_t& length) {
   return packed_character & ((-1ULL)>>(64-length*CHAR_BIT));
}

inline uint64_t construct(const char* str, std::size_t from) {
   return reinterpret_cast<const uint64_t*>(str+from)[0];
}
inline uint64_t construct(const char* str, std::size_t from, uint_fast8_t length) {
   DCHECK_GT(length,0);
   DCHECK_LE(length, FIT_CHARS);
   return truncate(construct(str,from), length);
}
inline uint64_t construct(const std::string& str, std::size_t from) {
   DCHECK_LT(from, str.length());
   return (from+ CHAR_BIT <= str.length()) ? construct(str.c_str(), from) : construct(str.c_str(), from, str.length()-from);
}

constexpr bool is_prefix(const uint64_t& a, const uint64_t& b) {
   return longest_common_prefix(a, b) == char_length(b);
}

}//ns


