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

#include<thread>
#include<functional>
#include <algorithm>

	template <class T>
	void DoNotOptimizeAway(T&& x)
	{
		static auto ttid = std::this_thread::get_id();
		if(ttid == std::thread::id())
		{
			// This forces the value to never be optimized away
			// by taking a reference then using it.
			const auto* p = &x;
			putchar(*reinterpret_cast<const char*>(p));

			// If we do get here, kick out because something has gone wrong.
			std::abort();
		}
	}


	template <class T>
	void DoNotOptimizeAway(std::function<T(void)>&& x)
	{
		volatile auto foo = x();

		static auto ttid = std::this_thread::get_id();
		if(ttid == std::thread::id())
		{
			// This forces the value to never be optimized away
			// by taking a reference then using it.
			const auto* p = &foo + &x;
			putchar(*reinterpret_cast<const char*>(p));

			// If we do get here, kick out because something has gone wrong.
			std::abort();
		}
	}
using lce_function_type = size_t (*)(const char*, size_t, const char*, size_t);

std::pair<size_t,size_t> measure_lce(const aligned_string& text, const aligned_string& text_dup, const size_t length, const lce_function_type& lce_func) {
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
    std::cout << "len " << length << " means " << math::arithmetic_mean(times) << " dev: " << math::deviation(times, math::arithmetic_mean(times)) << "LCE " << res.back() << std::endl;
    return std::make_pair(times[0], times.back());
}


const char* lce_name[] =
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




#ifdef NDEBUG
  constexpr size_t ITERATIONS = 100;
  constexpr size_t MIN_ITERATIONS = 20;
  constexpr size_t TEXT_LENGTH = 300;
#else
  constexpr size_t ITERATIONS = 10;
  constexpr size_t MIN_ITERATIONS = 5;
  constexpr size_t TEXT_LENGTH = 70;
#endif

std::vector<size_t> fill_best_until() {
  const aligned_string text(TEXT_LENGTH, 'a');
  const aligned_string text_dup(TEXT_LENGTH, 'a');
  std::vector<size_t> best_until(lce_functions, 0);


  size_t current_best_solution = 0;
  
  for(size_t length = 1; length < TEXT_LENGTH; ++length) {
    uint64_t time_lower_bound[lce_functions];
    uint64_t time_upper_bound[lce_functions];
    for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
      std::tie(time_lower_bound[function_counter], time_upper_bound[function_counter])  = measure_lce(text, text_dup, length, lce_function[function_counter]);
    }

    const size_t best_lower_solution = std::min_element(time_lower_bound,time_lower_bound+lce_functions) - time_lower_bound;
    const size_t best_upper_solution = std::min_element(time_upper_bound,time_upper_bound+lce_functions) - time_upper_bound;
    std::cout << lce_name[best_lower_solution] << " - " << lce_name[best_upper_solution] << std::endl;
    if(current_best_solution < best_lower_solution && time_upper_bound[best_lower_solution] <  time_lower_bound[current_best_solution] ) {
      for(size_t solution = current_best_solution; solution <= best_lower_solution; ++solution) {
	best_until[current_best_solution] = length;
      }
    }
    current_best_solution = best_lower_solution;
    best_until[current_best_solution] = length;
    if(current_best_solution == lce_functions-1) break;
  }
  return best_until;
}

#include <fstream>

int main(int argc, char *argv[]) {
  if(argc != 2) {
    std::cerr << "Usage: " << argv[0] << " [sourcefile to generate]" << std::endl;
    return 1;
  }
  std::ofstream os(argv[1]);

  size_t best_until[lce_functions];
  std::fill(best_until, best_until+lce_functions, 0);


  std::vector<std::vector<size_t>> best_untils;
  for(size_t best_untils_counter = 0; best_untils_counter < ITERATIONS; ++best_untils_counter) {
    best_untils.emplace_back(fill_best_until());
    size_t new_best_until[lce_functions];
    std::fill(new_best_until, new_best_until+lce_functions, 0);
    if(best_untils_counter > MIN_ITERATIONS) {
      for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
	std::vector<size_t> samples; 
	for(size_t sample_counter = 0; sample_counter < best_untils.size(); ++sample_counter) {
	  samples.push_back(best_untils[sample_counter][function_counter]);
	}
	std::sort(samples.begin(), samples.end());
	new_best_until[function_counter] = samples[samples.size()/2]; // take median;
      }
      bool stagnate = true;
      for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
	if(best_until[function_counter] != new_best_until[function_counter]) {
	  stagnate = false;
	  break;
	}
      }
      if(stagnate) break;
      for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
	best_until[function_counter] = new_best_until[function_counter];
      }

    }
  }



  os << "#include \"lcp.hpp\"" << std::endl;

  os << "namespace longest_common_prefix { " << std::endl;

    os << "size_t longest_common_prefix(const char* a, const size_t a_length, const char* b, const size_t b_length) {" << std::endl;

    for(size_t function_counter = 0; function_counter < lce_functions-1; ++function_counter) {
      if(function_counter > 0 && best_until[function_counter-1] > best_until[function_counter]) continue; // skip if previous solution is better
      os << "constexpr size_t THRESHOLD_" << lce_name[function_counter] << " = " << best_until[function_counter] << ";" << std::endl;
    }



    for(size_t function_counter = 0; function_counter < lce_functions-1; ++function_counter) {
      if(function_counter > 0 && best_until[function_counter-1] > best_until[function_counter]) continue; // skip if previous solution is better
      os << "if(a_length < THRESHOLD_" << lce_name[function_counter] << ") { " << std::endl;
      os << "return longest_common_prefix_" << lce_name[function_counter] << "(a, a_length, b, b_length);" << std::endl;
      os << "} else ";
    }
    os << "return longest_common_prefix_" << lce_name[lce_functions-1] << "(a, a_length, b, b_length);" << std::endl;

    os << "}" << std::endl;

  os << "} //namespace longest_common_prefix " << std::endl;
    os << std::endl;

  return 0;
}
