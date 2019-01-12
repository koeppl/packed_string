#include "precalc_lcp_common.cpp"

  constexpr size_t TEXT_LENGTH = 300;



#include <fstream>

int main(int argc, char *argv[]) {
  if(argc != 3) {
    std::cerr << "Usage: " << argv[0] << " [gnuplot-file] [plot-data]" << std::endl;
    return 1;
  }

  {
  std::ofstream os_gnu(argv[1]);

os_gnu << 
"#!/usr/bin/env gnuplot\n"
"set term svg enhanced mouse size 1024,400"
"set output 'lcp.svg'"
"set xrange [1:40]\n"
"set title 'longest common prefix'\n"
"set ylabel 'nanoseconds'\n"
"set xlabel 'string length'\n"
"set key outside\n"
"plot ";


for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
  os_gnu << "'" <<  argv[2] << "' using 1:($" << (function_counter*2+1) << "+$" << (function_counter*2) << "):($" << (function_counter*2+1) << "-$" << (function_counter*2) << ") with yerrorbars title '" << lce_name[function_counter] << "', ";
}
os_gnu << std::endl;
  }

  const aligned_string text(TEXT_LENGTH, 'a');
  const aligned_string text_dup(TEXT_LENGTH, 'a');

  std::ofstream os_plot(argv[2]);
    os_plot << "##length";
  for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
    os_plot << '\t' << lce_name[function_counter] << "_min" << '\t' << lce_name[function_counter] << "_max";
  }
  os_plot << std::endl;

    for(size_t length = 1; length < TEXT_LENGTH; ++length) {
      os_plot << length;
      for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
	uint64_t time_lower_bound;
	uint64_t time_upper_bound;
	std::tie(time_lower_bound, time_upper_bound)  = measure_lce(text, text_dup, length, lce_function[function_counter]);
	os_plot << '\t' << (static_cast<double>(time_lower_bound)/MEASUREMENTS) << "\t" << (static_cast<double>(time_upper_bound)/MEASUREMENTS);
    }
  os_plot << std::endl;

  }


  return 0;
}
