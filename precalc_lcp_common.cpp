#include <iostream>
#include "packed/lcp.hpp"

#include <chrono>
#include <vector>


namespace packed {

namespace math {

template<class T>
double arithmetic_mean(const std::vector<T>& v) {
  double sum = 0;
  for(const T& val : v) {
    sum += val;
  }
  return sum/v.size();
}

template<class T>
double deviation(const std::vector<T>& v, const double& mean) {
  double sum = 0;
  for(const T& val : v) {
    sum += (val - mean)*(val - mean);
  }
  return sum/v.size();
}

}//ns math
}//ns packed

#include <thread>
#include <functional>
#include <algorithm>

template <class T>
void DoNotOptimizeAway(T&& x) { // copied from https://github.com/DigitalInBlue/Celero/
  static auto ttid = std::this_thread::get_id();
  if(ttid == std::thread::id()) {
    const auto* p = &x;
    putchar(*reinterpret_cast<const char*>(p));
    std::abort();
  }
}


#ifdef NDEBUG
  constexpr size_t MEASUREMENTS = 1000;
  constexpr size_t MAX_SAMPLES = 500;
  constexpr size_t MIN_SAMPLES = 50;
  constexpr size_t MIN_DEVIATION = 10;
#else
  constexpr size_t MEASUREMENTS = 200;
  constexpr size_t MAX_SAMPLES = 100;
  constexpr size_t MIN_SAMPLES = 10;
  constexpr size_t MIN_DEVIATION = 30;
#endif


template<class experiment_type>
std::pair<size_t,size_t> experiment(experiment_type exp) {

    std::vector<uint64_t> times;
    std::vector<uint64_t> res;
    uint64_t prev_deviation = 0;

    for(size_t i = 0; i < MAX_SAMPLES; ++i) {
      res.push_back(0);

      res.back() = exp();

      using time_point = std::chrono::high_resolution_clock::time_point;
      time_point t1 = std::chrono::high_resolution_clock::now();

      for(size_t j = 0; j < MEASUREMENTS; ++j) {
	DoNotOptimizeAway(exp);
      }

      time_point t2 = std::chrono::high_resolution_clock::now();

      const uint64_t timespan = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

      times.insert(std::upper_bound(times.begin(), times.end(), timespan), timespan);

      if( (i+1) % 4 == 0) {
	times.pop_back();
	times.erase(times.begin());
      }
      if(times.size() > MIN_SAMPLES) {
	double deviation = packed::math::deviation(times, packed::math::arithmetic_mean(times));
	if(deviation < MIN_DEVIATION) break;
	if(prev_deviation > deviation) break;
      }
      prev_deviation = packed::math::deviation(times, packed::math::arithmetic_mean(times));
    }
    // std::cout << "len " << length << " means " << math::arithmetic_mean(times) << " dev: " << math::deviation(times, math::arithmetic_mean(times)) << "LCE " << res.back() << std::endl;
    return std::make_pair(times[0], times.back());
}


const char*const lcp_name[] =
  { "character"
  , "packed"
#ifdef __SSE2__
  , "sse"
#endif
#ifdef __AVX2__
  , "avx2_8"
  , "avx2"
#endif
#ifdef __AVX512__
  , "avx512"
#endif
};
  constexpr packed::lcp_function_type lcp_function[] = 
    { packed::longest_common_prefix_character
    , packed::longest_common_prefix_packed
#ifdef __SSE2__
    , packed::longest_common_prefix_sse
#endif
#ifdef __AVX2__
    , packed::longest_common_prefix_avx2_8
    , packed::longest_common_prefix_avx2
#endif
#ifdef __AVX512__
    , packed::longest_common_prefix_avx512
#endif
  };
  constexpr size_t lcp_functions = sizeof(lcp_function)/sizeof(packed::lcp_function_type);



const char*const lcp64_name[] =
  { "character"
#ifdef __SSE3__
  , "sse"
#endif
#ifdef __AVX2__
  , "avx2"
#endif
};
  constexpr packed::lcp64_function_type lcp64_function[] = 
    { packed::longest_common_prefix_character
#ifdef __SSE3__
    , packed::longest_common_prefix_sse
#endif
#ifdef __AVX2__
    , packed::longest_common_prefix_avx2
#endif
  };
  constexpr size_t lcp64_functions = sizeof(lcp64_function)/sizeof(packed::lcp64_function_type);


struct lcp_experiment {
  using function_type = packed::lcp_function_type;

    const packed::aligned_string& text;
    const packed::aligned_string& text_dup;
    const size_t length;
    const function_type& lcp_func;

    static constexpr size_t function_array_size = lcp_functions;
    static constexpr auto function_array = lcp_function;

    size_t operator()() const {
      return lcp_func(text.data(), text_dup.data(), length);
    }

};
struct lcp64_experiment {
  using function_type = packed::lcp64_function_type;

    const packed::aligned_string& text;
    const packed::aligned_string& text_dup;
    const size_t length;
    const function_type& lcp_func;

    static constexpr auto function_array = lcp64_function;
    static constexpr size_t function_array_size = lcp64_functions;

    size_t operator()() const {
      return lcp_func(reinterpret_cast<const uint64_t*>(text.data()), reinterpret_cast<const uint64_t*>(text_dup.data()), length/sizeof(uint64_t));
    }

};
