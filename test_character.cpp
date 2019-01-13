#include "character.hpp"
#include "random_string.hpp"

#include <gtest/gtest.h>



void test_packed_character(const std::string& input) {
   const size_t packed_string_length = packed::math::ceil_div(input.length(),packed::character::FIT_CHARS);
   uint64_t* packed_string = new uint64_t[packed_string_length];
   for(size_t i = 0; i < packed_string_length; ++i) {
      packed_string[i] = packed::character::construct(input,i*packed::character::FIT_CHARS);
      for(size_t j = 0; i*packed::character::FIT_CHARS+j < input.length() && j < packed::character::FIT_CHARS; ++j) {
         ASSERT_EQ(packed::character::character(packed_string[i],j), input[i*packed::character::FIT_CHARS+j]);
      }
   }

   //select a random packed char 
   //
   const size_t packed_index = random_size(input.length()/packed::character::FIT_CHARS);
   const uint64_t packed_char = packed_string[packed_index];
   const uint_fast8_t packed_length = packed::character::char_length(packed_char);
   if(input.length() == 0) {
      ASSERT_EQ(packed_char, 0);
   }
   else if(packed_length != packed::character::FIT_CHARS) {
      ASSERT_EQ(packed_length, input.length()-(packed_string_length-1)*packed::character::FIT_CHARS);
   }


   const size_t begin  = random_size(packed_length);
   const size_t length = random_size(packed_length-begin)+1;
   const uint64_t sub_char = packed::character::substring(packed_char, begin, length);
#ifndef NDEBUG
   const uint_fast8_t sub_length = packed::character::char_length(sub_char);
   ASSERT_EQ(sub_length, length);
#endif

   ASSERT_EQ(sub_char, packed::character::construct(input.c_str(),packed_index*packed::character::FIT_CHARS+begin, length));

   // if(sub_length < length) {
   //    ASSERT_EQ(packed_index, packed_string_length);
   //    ASSERT_EQ(packed::character::char_length(sub_char), input.length() - (packed_string_length-1)*packed::character::FIT_CHARS);
   // }
   for(size_t j = 0; j < packed::character::char_length(sub_char); ++j) {
      ASSERT_EQ(packed::character::character(sub_char, j), input[packed_index*packed::character::FIT_CHARS+begin+j]);
   }
   delete [] packed_string;
}

constexpr size_t TEST_LENGTH = 100;
constexpr size_t TEST_REPS = 1000;
TEST(packed, overall) { 
   random_char rnd_gen;
   for(size_t test_length = 1; test_length < TEST_LENGTH; ++test_length) {
      for(size_t test_reps = 1; test_reps < TEST_REPS; ++test_reps) {
         const std::string input = random_string(rnd_gen,test_length);
         test_packed_character(input);
      }
   } 
}


TEST(packed, character) { 
   random_char rnd_gen;
   for(size_t test_length = 0; test_length < 20; ++test_length) {
      for(size_t test_position = 0; test_position < test_length; ++test_position) {
         const std::string input = random_string(rnd_gen,test_length);
         uint64_t packed = packed::character::construct(input, test_position);
         for(size_t i = 0; i < 8 && test_position+i < input.length(); ++i) {
            ASSERT_EQ(packed::character::character(packed, i), input[test_position+i]);
         }
      }

   } 
}

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
