#include <glog/logging.h>
#include <climits>

//! computes ceil(dividend/divisor)
template<class T>
constexpr T ceil_div(const T& dividend, const T& divisor) { 
   return (dividend + divisor - 1) / divisor;
}

constexpr int most_significant_bit(const uint64_t& x) {
    return x == 0 ? -1 : (sizeof(uint64_t)*8-1) - __builtin_clzll(x);
}

namespace packed_character {
#ifdef NDEBUG
#define __constexpr contexpr
#else
#define __constexpr 
#endif

static constexpr size_t FIT_CHARS = sizeof(uint64_t)/sizeof(char);

//! number of characters stored in packed_character a
constexpr uint_fast8_t char_size(const uint64_t &a) {
   return a == 0 ? 0 : (most_significant_bit(a)/CHAR_BIT + 1);
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
	std::string s(length+1, '\0');
	std::generate_n(s.begin(), length, rnd_gen);
	// s[length] = '\0';
	return s;
}

int main() {
   random_char rnd_gen;
   while(1) {
      std::string s = random_string(rnd_gen, 35);
      const size_t section_length = s.length()/packed_character::FIT_CHARS;
      uint64_t* arr = new uint64_t[section_length];
      for(size_t i = 0; i < s.length()/packed_character::FIT_CHARS; ++i) {
         arr[i] = packed_character::construct(s.c_str(),i*packed_character::FIT_CHARS);
         for(size_t j = 0; j < packed_character::FIT_CHARS; ++j) {
            DCHECK_EQ(packed_character::character(arr[i],j), s[i*packed_character::FIT_CHARS+j]);
         }
      }
      const size_t section = random_size(s.length()/packed_character::FIT_CHARS);
      const size_t begin = random_size(packed_character::FIT_CHARS);
      const size_t length = random_size(packed_character::FIT_CHARS-begin)+1;
      const uint64_t sub = packed_character::substring(arr[section], begin, length);

      DCHECK_EQ(sub, packed_character::construct(s.c_str(),section*packed_character::FIT_CHARS+begin, length));

      if(packed_character::char_size(sub) < length) {
         DCHECK_EQ(section, section_length);
         DCHECK_EQ(packed_character::char_size(sub), s.length() - (section_length-1)*packed_character::FIT_CHARS);
      }
      for(size_t j = 0; j < packed_character::char_size(sub); ++j) {
         DCHECK_EQ(packed_character::character(sub, j), s[section*packed_character::FIT_CHARS+begin+j]);
      }
      

      // for(size_t i = 0; i < s.length()/packed_character::FIT_CHARS; ++i) {
      //    for(size_t j = 0; j < packed_character::FIT_CHARS; ++j) {
      //       DCHECK_EQ(packed_character::character(arr[i],j), s[i*packed_character::FIT_CHARS+j]);
      //    }

         delete [] arr;
      }
   }



