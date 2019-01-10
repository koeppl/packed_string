#include <celero/Celero.h>
#include "random_string.hpp"
#include "packed_string.hpp"

CELERO_MAIN
random_char rnd_gen;


BASELINE(DemoSimple, Baseline, 500, 100000)
{
   const std::string input = random_string(rnd_gen, 10);
   const uint64_t a = packed_character::construct(input,0);
//   celero::DoNotOptimizeAway(charSize(a));
   uint32_t from = 0;
   uint32_t to = 7;
   uint32_t length = 7;
   celero::DoNotOptimizeAway(packed_character::substring(a, from, length));
}

BENCHMARK(DemoSimple, Complex1, 500, 100000)
{
   const std::string input = random_string(rnd_gen, 10);
   const uint64_t a = packed_character::construct(input,0);
   uint32_t from = 0;
   uint32_t to = 7;
   uint32_t length = 7;
   //celero::DoNotOptimizeAway(packed_character::char_length(a));
   celero::DoNotOptimizeAway(toSubLong(a, from, to));
}
BENCHMARK(DemoSimple, Complex2, 500, 100000)
{
   const std::string input = random_string(rnd_gen, 10);
   const uint64_t a = packed_character::construct(input,0);
   //celero::DoNotOptimizeAway(packed_character::char_length(a));
   uint32_t from = 0;
   uint32_t to = 7;
   uint32_t length = 7;
   celero::DoNotOptimizeAway(substring2(a, from, length));
}

BENCHMARK(DemoSimple, Complex3, 500, 100000)
{
   const std::string input = random_string(rnd_gen, 10);
   const uint64_t a = packed_character::construct(input,0);
   //celero::DoNotOptimizeAway(packed_character::char_length(a));
   uint32_t from = 0;
   uint32_t to = 7;
   uint32_t length = 7;
   celero::DoNotOptimizeAway(substring3(a, from, length));
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



