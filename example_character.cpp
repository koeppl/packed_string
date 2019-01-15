#include "packed/character.hpp"
#include "random_string.hpp"

#include <iostream>

namespace C = packed::character;

void print_packed(uint64_t packed) {
   for(size_t i = 0; i < C::FIT_CHARS; ++i) {
      std::cout << C::character(packed, i);
   }
   std::cout << std::endl;
}

int main() {
   std::string str = "Hello World!";
   auto packed = C::construct(str, 6); // packed = str[6..]
   std::cout << "str = " << str << std::endl;
   std::cout << "packed = str[6..] = ";
   print_packed(packed);
   std::cout << "|packed| = " << static_cast<size_t>(C::char_length(packed)) << std::endl; 
   auto sub = C::substring(packed, 2,4); // sub = packed[2..2+3]
   std::cout << "sub = packed[2..2+3] = ";
   print_packed(sub); 
   std::cout << "|sub| = " << static_cast<size_t>(C::char_length(sub)) << std::endl; 
   std::cout << "lcp(packed,sub) = " << static_cast<size_t>(C::longest_common_prefix(packed, sub)) << std::endl;

   std::cout << "lcp(str[2..],str[9..]) = " << static_cast<size_t>(C::longest_common_prefix(C::construct(str,2), C::construct(str,9))) << std::endl;

   auto prefix = C::truncate(packed, 4); // prefix = packed[1..4]
   std::cout << "prefix = packed[1..4] = ";
   print_packed(prefix); 
   std::cout << "|prefix| = " << static_cast<size_t>(C::char_length(sub)) << std::endl; 
   std::cout << "is prefix?: " << C::is_prefix(packed, prefix) << std::endl; 


}
