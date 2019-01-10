#include "packed_string.hpp"

int main() {
   random_char rnd_gen;
   while(1) {
      std::string input = random_string(rnd_gen, 77);
      const size_t pos = random_size(13);
      uint64_t packed = packed_character::construct(input, pos);
      for(size_t i = 0; i < 8; ++i) {
         DCHECK_EQ(packed_character::character(packed, i), input[pos+i]);
      }
      // test_packed_character(input);


      // for(size_t i = 0; i < s.length()/packed_character::FIT_CHARS; ++i) {
      //    for(size_t j = 0; j < packed_character::FIT_CHARS; ++j) {
      //       DCHECK_EQ(packed_character::character(arr[i],j), s[i*packed_character::FIT_CHARS+j]);
      //    }

      }
   }



