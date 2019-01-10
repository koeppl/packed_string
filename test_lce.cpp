#include <iostream>
#include "lce.hpp"

int main() {

   while( 1 ) {
      random_char rnd_gen;
      std::string input = random_string(rnd_gen, random_size(100000)+1);
      char* in1 = reinterpret_cast<char*>(aligned_alloc(32, input.size()));
      char* in2 = reinterpret_cast<char*>(aligned_alloc(32, input.size()));
      for(size_t i = 0; i < input.size(); ++i) {
	 in1[i] = in2[i] = input[i];
      }

      const size_t pos = random_size(input.size()); //input.size());
      in2[pos] = in1[pos]+1;


      // DCHECK_EQ(longest_common_prefix_packed(in1,input.size(), in2, input.size()), pos);
      // DCHECK_EQ(longest_common_prefix(in1,in2), pos);
      //std::cout << longest_common_prefix(in1,in2) << " : "  <<  longest_common_prefix_avx2(in1,input.length(), in2, input.length()) << std::endl;
      DCHECK_EQ(longest_common_prefix_naive(in1,in2), longest_common_prefix_sse(in1,input.length(), in2, input.length()));
   //   DCHECK_EQ(longest_common_prefix(in1,in2), longest_common_prefix(std::string(in1),std::string(in2)));

      free(in1);
      free(in2);
   }
}
