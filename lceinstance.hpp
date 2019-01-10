#pragma once
#include "random_string.hpp"

class LCEInstance {
   public:
      const size_t m_length;
      const size_t m_position;
      char*const m_stra;
      char*const m_strb;
      size_t index;

      /** An instance to test, where there is a difference at position `position` in two random strings of the length `length` **/
      LCEInstance(size_t length, size_t position)
	 : m_length(length)
	   , m_position(position)
	   , m_stra(reinterpret_cast<char*>(aligned_alloc(32, m_length+1)))
	   , m_strb(reinterpret_cast<char*>(aligned_alloc(32, m_length+1)))

   {
      random_char rnd_gen;
      std::string input = random_string(rnd_gen, m_length);
      for(size_t i = 0; i < input.size(); ++i) {
	 m_stra[i] = m_strb[i] = input[i];
      }
      m_stra[length] = m_strb[length] = 0;
      m_strb[m_position] = m_stra[m_position]+1;
   }
      // ~LCEInstance() {
	//  free(m_stra);
	//  free(m_strb);
      // }

};
