#pragma once 

#include <iostream>
#include <memory>
#include <cstdlib>

namespace packed {

/**
 * @brief computes the longest common prefix of `a` and `b`
 * @a first string
 * @b second string
 * Both strings a and b must be 32-byte aligned and are treated as 64-bit characters
 * @length number of characters to compare, which could be std::min(strlen(a),strlen(b))
 */
size_t longest_common_prefix_character(const uint64_t*const a, const uint64_t*const b, const size_t length);

#ifdef __SSE3__
size_t longest_common_prefix_sse(const uint64_t*const a, const uint64_t*const b, const size_t length);
#endif

#ifdef __AVX2__
size_t longest_common_prefix_avx2(const uint64_t*const a, const uint64_t*const b, const size_t length);
#endif


/**
 * @brief computes the longest common prefix of `a` and `b`
 * @a first string
 * @b second string
 * Both strings a and b must be 32-byte aligned
 * @length number of characters to compare, which could be std::min(strlen(a),strlen(b))
 */
size_t longest_common_prefix(const char* a, const char* b, const size_t length);

/** computes the longest common prefix by character comparison
 * @see longest_common_prefix
 */
size_t longest_common_prefix_character(const char* a, const char* b, const size_t length);

/** computes the longest common prefix by comparing packed characters
 * @see longest_common_prefix
 */
size_t longest_common_prefix_packed(const char* a, const char* b, const size_t length);

#ifdef __SSE2__
/** computes the longest common prefix by using SSE2 instructions
 * Both strings a and b must be 16-byte aligned
 * @see longest_common_prefix
 */
size_t longest_common_prefix_sse(const char*const a, const char*const b, const size_t length);
#endif

#ifdef __AVX2__
/** computes the longest common prefix by using AVX2 instructions
 * Both strings a and b must be 16-byte aligned
 * @see longest_common_prefix
 */
size_t longest_common_prefix_avx2(const char*const a, const char*const b, const size_t length);
#endif

#ifdef __AVX512__
/** computes the longest common prefix by using AVX512 instructions
 * Both strings a and b must be 16-byte aligned
 * @see longest_common_prefix
 */
size_t longest_common_prefix_avx512(const char*const a, const char*const b, const size_t length);
#endif



using lcp_function_type = size_t (*)(const char*const, const char*const, const size_t);
using lcp64_function_type = size_t (*)(const uint64_t*const, const uint64_t*const, const size_t);

/**
 *  Computes lcp(a[offset_a..length], b[offset_b..length])
 *  `a` and `b` need to be 32-bit aligned, and |offset_a - offset_b| mod 4 = 0, such that we can 
 *  find an integer j such that a[offset_a+j..]  and b[offset_b+j..] are 32-byte aligned.
 */
size_t longest_common_prefix_offset(const uint64_t*const a, const size_t offset_a, const uint64_t*const b, const size_t offset_b, const size_t length, const lcp64_function_type& lcp);


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
#ifdef __APPLE__
	pointer p;
	posix_memalign(reinterpret_cast<void**>(&p), alignment, n*sizeof(T));
	return p;
#else
	return static_cast<pointer>(aligned_alloc(alignment, n*sizeof(T)));
#endif
     }
};

//! specialization of std::string to make the underlying char*-string 32-byte aligned
using aligned_string = std::basic_string<char, std::char_traits<char>, aligned_allocator<char>>;

}//ns
