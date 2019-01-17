#pragma once 

#include <iostream>
#include <memory>
#include <cstdlib>

namespace packed {

/**
 * @brief computes the longest common prefix of `a` and `b`
 * @a first string
 * @a_length number of characters of a
 * @b second string
 * @b_length number of characters of b
 */
size_t longest_common_prefix(const char* a, const size_t a_length, const char* b, const size_t b_length);

//! computes the longest common prefix by character comparison
size_t longest_common_prefix_character(const char* a, const size_t a_length, const char* b, const size_t b_length);

//! computes the longest common prefix by comparing packed characters
size_t longest_common_prefix_packed(const char* a, const size_t a_length, const char* b, const size_t b_length);

#ifdef __SSE2__
//! computes the longest common prefix by using SSE2 instructions
size_t longest_common_prefix_sse(const char*const a, const size_t a_length, const char*const b, const size_t b_length);
#endif

#ifdef __AVX2__
//! computes the longest common prefix by using AVX2 instructions
size_t longest_common_prefix_avx2(const char*const a, const size_t a_length, const char*const b, const size_t b_length);
#endif

#ifdef __AVX512__
//! computes the longest common prefix by using AVX512 instructions
size_t longest_common_prefix_avx512(const char*const a, const size_t a_length, const char*const b, const size_t b_length);
#endif


/**
 * The functions working with SSE2, AVX2 and AVX512 instruction sets expect that the strings `a` and `b` are 32-byte aligned
 * This can be achieved with allocating memory with `aligned_alloc`.
 * For std::string (or other C++ containers), we can use the allocator `aligned_allocator` to do the job.
 */
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

//! specialization of std::string to make the underlying char*-string 32-byte aligned
using aligned_string = std::basic_string<char, std::char_traits<char>, aligned_allocator<char>>;

}//ns
