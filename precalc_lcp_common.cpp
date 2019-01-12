#include <iostream>
#include "lcp.cpp"

#include <chrono>
#include <vector>


using namespace longest_common_prefix;


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

}

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

using lce_function_type = size_t (*)(const char*, size_t, const char*, size_t);

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
std::pair<size_t,size_t> measure_lce(const aligned_string& text, const aligned_string& text_dup, const size_t length, const lce_function_type& lce_func) {

    std::vector<uint64_t> times;
    std::vector<uint64_t> res;
    uint64_t prev_deviation = 0;

    for(size_t i = 0; i < MAX_SAMPLES; ++i) {
      res.push_back(0);

      res.back() = longest_common_prefix_sse(text.data(), length, text_dup.data(), length);

      using time_point = std::chrono::high_resolution_clock::time_point;
      time_point t1 = std::chrono::high_resolution_clock::now();

      for(size_t j = 0; j < MEASUREMENTS; ++j) {
	DoNotOptimizeAway(lce_func(text.data(), length, text_dup.data(), length));
      }

      time_point t2 = std::chrono::high_resolution_clock::now();

      const uint64_t timespan = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

      times.insert(std::upper_bound(times.begin(), times.end(), timespan), timespan);

      if( (i+1) % 4 == 0) {
	times.pop_back();
	times.erase(times.begin());
      }
      if(times.size() > MIN_SAMPLES) {
	double deviation = math::deviation(times, math::arithmetic_mean(times));
	if(deviation < MIN_DEVIATION) break;
	if(prev_deviation > deviation) break;
      }
      prev_deviation = math::deviation(times, math::arithmetic_mean(times));
    }
    // std::cout << "len " << length << " means " << math::arithmetic_mean(times) << " dev: " << math::deviation(times, math::arithmetic_mean(times)) << "LCE " << res.back() << std::endl;
    return std::make_pair(times[0], times.back());
}


const char*const lce_name[] =
  { "character"
  , "packed"
#ifdef __SSE2__
  , "sse"
#endif
#ifdef __AVX2__
  , "avx2"
#endif
#ifdef __AVX512__
  , "avx512"
#endif
};
  constexpr lce_function_type lce_function[] = 
    { longest_common_prefix_character
    , longest_common_prefix_packed
#ifdef __SSE2__
    , longest_common_prefix_sse
#endif
#ifdef __AVX2__
    , longest_common_prefix_avx2
#endif
#ifdef __AVX512__
    , longest_common_prefix_avx512
#endif
  };
  constexpr size_t lce_functions = sizeof(lce_function)/sizeof(lce_function_type);

