#include "precalc_lcp_common.cpp"
#include "lcp.cpp"



#ifdef NDEBUG
  constexpr size_t ITERATIONS = 100;
  constexpr size_t MIN_ITERATIONS = 20;
  constexpr size_t TEXT_LENGTH = 300;
#else
  constexpr size_t ITERATIONS = 10;
  constexpr size_t MIN_ITERATIONS = 5;
  constexpr size_t TEXT_LENGTH = 70;
#endif


template<class experiment_type>
std::vector<size_t> fill_best_until() {
  constexpr auto function_array = experiment_type::function_array;
  constexpr size_t function_array_size = experiment_type::function_array_size;
  const packed::aligned_string text(TEXT_LENGTH, 'a');
  const packed::aligned_string text_dup(TEXT_LENGTH, 'a');
  std::vector<size_t> best_until(function_array_size, 0);


  size_t current_best_solution = 0;
  
  for(size_t length = 1; length < TEXT_LENGTH; ++length) {
    uint64_t time_lower_bound[function_array_size];
    uint64_t time_upper_bound[function_array_size];
    for(size_t function_counter = 0; function_counter < function_array_size; ++function_counter) {
      experiment_type exp { text, text_dup, length, function_array[function_counter] };
      std::tie(time_lower_bound[function_counter], time_upper_bound[function_counter])  = experiment(exp);
    }

    const size_t best_lower_solution = std::min_element(time_lower_bound,time_lower_bound+function_array_size) - time_lower_bound;
    if(current_best_solution < best_lower_solution && time_upper_bound[best_lower_solution] <  time_lower_bound[current_best_solution] ) {
      for(size_t solution = current_best_solution; solution <= best_lower_solution; ++solution) {
	best_until[current_best_solution] = length;
      }
    }
    current_best_solution = best_lower_solution;
    best_until[current_best_solution] = length;
    if(current_best_solution == function_array_size-1) break;
  }
  return best_until;
}

template<class experiment_type>
std::vector<size_t> best_until_wrapper() {
  constexpr size_t function_array_size = experiment_type::function_array_size;
  std::vector<size_t> best_until(function_array_size,0);
  // size_t best_until[function_array_size];
  // std::fill(best_until, best_until+function_array_size, 0);
  std::vector<std::vector<size_t>> best_untils;
  for(size_t best_untils_counter = 0; best_untils_counter < ITERATIONS; ++best_untils_counter) {
    best_untils.emplace_back(fill_best_until<experiment_type>());
    size_t new_best_until[function_array_size];
    std::fill(new_best_until, new_best_until+function_array_size, 0);
    if(best_untils_counter > MIN_ITERATIONS) {
      for(size_t function_counter = 0; function_counter < function_array_size; ++function_counter) {
	std::vector<size_t> samples; 
	for(size_t sample_counter = 0; sample_counter < best_untils.size(); ++sample_counter) {
	  samples.push_back(best_untils[sample_counter][function_counter]);
	}
	std::sort(samples.begin(), samples.end());
	new_best_until[function_counter] = samples[samples.size()/2]; // take median;
      }
      bool stagnate = true;
      for(size_t function_counter = 0; function_counter < function_array_size; ++function_counter) {
	if(best_until[function_counter] != new_best_until[function_counter]) {
	  stagnate = false;
	  break;
	}
      }
      if(stagnate) break;
      for(size_t function_counter = 0; function_counter < function_array_size; ++function_counter) {
	best_until[function_counter] = new_best_until[function_counter];
      }
    }
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



  os << "#include \"lcp.hpp\"" << std::endl;

  os << "namespace packed { " << std::endl;

  {
  const std::vector<size_t> best_until = best_until_wrapper<lcp_experiment>();
    os << "size_t longest_common_prefix(const char*const a, const char*const b, const size_t length) {" << std::endl;

    for(size_t function_counter = 0; function_counter < lcp_functions-1; ++function_counter) {
      if(function_counter > 0 && best_until[function_counter-1] > best_until[function_counter]) continue; // skip if previous solution is better
      os << "constexpr size_t THRESHOLD_" << lcp_name[function_counter] << " = " << best_until[function_counter] << ";" << std::endl;
    }

    for(size_t function_counter = 0; function_counter < lcp_functions-1; ++function_counter) {
      if(function_counter > 0 && best_until[function_counter-1] > best_until[function_counter]) continue; // skip if previous solution is better
      os << "if(length < THRESHOLD_" << lcp_name[function_counter] << ") { " << std::endl;
      os << "return longest_common_prefix_" << lcp_name[function_counter] << "(a, b, length);" << std::endl;
      os << "} else ";
    }
    os << "return longest_common_prefix_" << lcp_name[lcp_functions-1] << "(a, b, length);" << std::endl;

    os << "}" << std::endl;
  }
  {
  const std::vector<size_t> best_until = best_until_wrapper<lcp64_experiment>();
    os << "size_t longest_common_prefix(const uint64_t*const a, const uint64_t*const b, const size_t length) {" << std::endl;

    for(size_t function_counter = 0; function_counter < lcp64_functions-1; ++function_counter) {
      if(function_counter > 0 && best_until[function_counter-1] > best_until[function_counter]) continue; // skip if previous solution is better
      os << "constexpr size_t THRESHOLD_" << lcp64_name[function_counter] << " = " << best_until[function_counter] << ";" << std::endl;
    }

    for(size_t function_counter = 0; function_counter < lcp64_functions-1; ++function_counter) {
      if(function_counter > 0 && best_until[function_counter-1] > best_until[function_counter]) continue; // skip if previous solution is better
      os << "if(length < THRESHOLD_" << lcp64_name[function_counter] << ") { " << std::endl;
      os << "return longest_common_prefix_" << lcp64_name[function_counter] << "(a, b, length);" << std::endl;
      os << "} else ";
    }
    os << "return longest_common_prefix_" << lcp64_name[lcp64_functions-1] << "(a, b, length);" << std::endl;

    os << "}" << std::endl;
  }

  os << "} //namespace packed " << std::endl;
    os << std::endl;

  return 0;
}
