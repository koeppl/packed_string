#include <glog/logging.h>
#include <climits>

namespace math {

//! computes ceil(dividend/divisor)
template<class T>
constexpr T ceil_div(const T& dividend, const T& divisor) { 
   return (dividend + divisor - 1) / divisor;
}

constexpr int most_significant_bit(const uint64_t& x) {
    return x == 0 ? -1 : (sizeof(uint64_t)*8-1) - __builtin_clzll(x);
}

}//ns

namespace packed_character {
#ifdef NDEBUG
#define __constexpr contexpr
#else
#define __constexpr 
#endif

static constexpr size_t FIT_CHARS = sizeof(uint64_t)/sizeof(char);

//! number of characters stored in packed_character a
constexpr uint_fast8_t char_size(const uint64_t &a) {
   return a == 0 ? 0 : (math::most_significant_bit(a)/CHAR_BIT + 1);
}

//! longest common prefix of two packed characters
constexpr uint_fast8_t longest_common_prefix(const uint64_t &a, const uint64_t &b) {
   return (a == b ?  char_size(a) : (__builtin_ctzll(a ^ b) >> 3));
}

//! retrieve character stored in packed_character
__constexpr char character(const uint64_t& packed_character, const size_t& index) {
   DCHECK_LT(index, sizeof(uint64_t)/sizeof(char));
   return (packed_character >> (index * CHAR_BIT)) & UCHAR_MAX;
}

__constexpr uint64_t substring(const uint64_t &packed_character, const uint_fast8_t& from, const uint_fast8_t& length) {
   DCHECK_GT(length,0);
   DCHECK_LE(from+length, char_size(packed_character));
   return (packed_character >> (from * CHAR_BIT)) & ((-1ULL)>>(64-length*CHAR_BIT));
}

constexpr uint64_t truncate(const uint64_t& packed_character, const uint_fast8_t& length) {
   return packed_character & ((-1ULL)>>(64-length*CHAR_BIT));
}

uint64_t construct(const char* str, size_t from) {
   return reinterpret_cast<const uint64_t*>(str+from)[0];
}
uint64_t construct(const char* str, size_t from, uint_fast8_t length) {
   DCHECK_GT(length,0);
   DCHECK_LE(length, FIT_CHARS);
   return truncate(construct(str,from), length);
}
uint64_t construct(const std::string& str, size_t from) {
   DCHECK_LT(from, str.length());
   return (from+ CHAR_BIT <= str.length()) ? construct(str.c_str(), from) : construct(str.c_str(), from, str.length()-from);
      // : construct(str.c_str(), from) & ((-1ULL)>>(64-(str.length()-from)*CHAR_BIT));
}

}//ns


#include <algorithm>
#include <iostream>

size_t random_size(const size_t& maxvalue) {
   return static_cast<std::size_t>(std::rand() * (1.0 / (RAND_MAX + 1.0 )) * maxvalue);
}

struct random_char{
    random_char(char const* range = "abcdefghijklmnopqrstuvwxyz0123456789")
        : m_range(range), m_length(strlen(m_range)) { }

    char operator ()() const {
        return m_range[random_size(m_length)];
    }

private:
    char const*const m_range;
    const std::size_t m_length;
};

std::string random_string(const random_char& rnd_gen, size_t length) {
	std::string s(length, '\0');
	std::generate_n(s.begin(), length, rnd_gen);
	// s[length] = '\0';
	return s;
}


void test_packed_character(const std::string& input) {
   const size_t packed_string_length = ceil_div(input.length(),packed_character::FIT_CHARS);
   uint64_t* packed_string = new uint64_t[packed_string_length];
   for(size_t i = 0; i < packed_string_length; ++i) {
      packed_string[i] = packed_character::construct(input,i*packed_character::FIT_CHARS);
      for(size_t j = 0; i*packed_character::FIT_CHARS+j < input.length() && j < packed_character::FIT_CHARS; ++j) {
         DCHECK_EQ(packed_character::character(packed_string[i],j), input[i*packed_character::FIT_CHARS+j]);
      }
   }

   //select a random packed char 
   //
   const size_t packed_index = random_size(input.length()/packed_character::FIT_CHARS);
   const uint64_t packed_char = packed_string[packed_index];
   const uint_fast8_t packed_length = packed_character::char_size(packed_char);
   if(packed_length != packed_character::FIT_CHARS) {
      DCHECK_EQ(packed_length, input.length()-(packed_string_length-1)*packed_character::FIT_CHARS);
   }


   const size_t begin  = random_size(packed_length);
   const size_t length = random_size(packed_length-begin)+1;
   const uint64_t sub_char = packed_character::substring(packed_char, begin, length);
   const uint_fast8_t sub_length = packed_character::char_size(sub_char);
   DCHECK_EQ(sub_length, length);

   DCHECK_EQ(sub_char, packed_character::construct(input.c_str(),packed_index*packed_character::FIT_CHARS+begin, length));

   // if(sub_length < length) {
   //    DCHECK_EQ(packed_index, packed_string_length);
   //    DCHECK_EQ(packed_character::char_size(sub_char), input.length() - (packed_string_length-1)*packed_character::FIT_CHARS);
   // }
   for(size_t j = 0; j < packed_character::char_size(sub_char); ++j) {
      DCHECK_EQ(packed_character::character(sub_char, j), input[packed_index*packed_character::FIT_CHARS+begin+j]);
   }
   delete [] packed_string;
}


int main() {
   random_char rnd_gen;
   while(1) {
      std::string input = random_string(rnd_gen, random_size(1000)+1);
      test_packed_character(input);
      

      // for(size_t i = 0; i < s.length()/packed_character::FIT_CHARS; ++i) {
      //    for(size_t j = 0; j < packed_character::FIT_CHARS; ++j) {
      //       DCHECK_EQ(packed_character::character(arr[i],j), s[i*packed_character::FIT_CHARS+j]);
      //    }

      }
   }



