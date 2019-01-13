#pragma once 

#include <iostream>
#include <memory>

namespace packed {

size_t longest_common_prefix(const char* a, const size_t a_length, const char* b, const size_t b_length);


size_t longest_common_prefix_character(const char* a, const size_t a_length, const char* b, const size_t b_length);
size_t longest_common_prefix_packed(const char* a, const size_t a_length, const char* b, const size_t b_length);

#ifdef __SSE2__
size_t longest_common_prefix_sse(const char*const a, const size_t a_length, const char*const b, const size_t b_length);
#endif

#ifdef __AVX2__
size_t longest_common_prefix_avx2(const char*const a, const size_t a_length, const char*const b, const size_t b_length);
#endif

#ifdef __AVX512__
size_t longest_common_prefix_avx512(const char*const a, const size_t a_length, const char*const b, const size_t b_length);
#endif


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
using aligned_string = std::basic_string<char, std::char_traits<char>, aligned_allocator<char>>;

}//ns
