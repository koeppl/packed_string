#include "precalc_lcp_common.cpp"
#include <fstream>



int main(int argc, char *argv[]) {
  if(argc != 4) {
    std::cerr << "Usage: " << argv[0] << " [gnuplot-file] [plot-data] [length]" << std::endl;
    return 1;
  }
  const size_t text_length = strtoul(argv[3], NULL, 10);

  {//write gnuplot file
  std::ofstream os_gnu(argv[1]);

os_gnu << 
"#!/usr/bin/env gnuplot\n"
"#set term svg enhanced mouse size 1024,400\n"
"#set output 'lcp.svg'\n"
"#set xrange [1:40]\n"
"set yrange [1:40]\n"
"set title 'longest common prefix'\n"
"set ylabel 'nanoseconds'\n"
"set xlabel 'string length'\n"
"set key outside\n"
"plot ";


//compute plot data
for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
  const size_t min_line = (function_counter*2+2);
  const size_t max_line = (function_counter*2+3);
  os_gnu << "'" <<  argv[2] << "' using 1:(($" << min_line << "+$" << max_line << ")/2):($" << max_line << "-$" << min_line << ") with yerrorbars title '" << lce_name[function_counter] << "', ";
}
  const size_t min_line = (lce_functions*2+1);
  const size_t max_line = (lce_functions*2+2);
  os_gnu << "'" <<  argv[2] << "' using 1:(($" << min_line << "+$" << max_line << ")/2):($" << max_line << "-$" << min_line << ") with yerrorbars title '" << "precomputed" << "', ";
os_gnu << std::endl;
  }

  const packed::aligned_string text(text_length, 'a');
  const packed::aligned_string text_dup(text_length, 'a');

  std::ofstream os_plot(argv[2]);
    os_plot << "##length";
  for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
    os_plot << '\t' << lce_name[function_counter] << "_min" << '\t' << lce_name[function_counter] << "_max";
  }
    os_plot << '\t' << "precomputed" << "_min" << '\t' << "precomputed" << "_max";
  os_plot << std::endl;

    for(size_t length = 1; length < text_length; ++length) {
      os_plot << length;
      for(size_t function_counter = 0; function_counter < lce_functions; ++function_counter) {
	uint64_t time_lower_bound;
	uint64_t time_upper_bound;
	std::tie(time_lower_bound, time_upper_bound)  = measure_lce(text, text_dup, length, lce_function[function_counter]);
	os_plot << '\t' << (static_cast<double>(time_lower_bound)/MEASUREMENTS) << "\t" << (static_cast<double>(time_upper_bound)/MEASUREMENTS);
    }
	uint64_t time_lower_bound;
	uint64_t time_upper_bound;
	std::tie(time_lower_bound, time_upper_bound)  = measure_lce(text, text_dup, length, packed::longest_common_prefix);
	os_plot << '\t' << (static_cast<double>(time_lower_bound)/MEASUREMENTS) << "\t" << (static_cast<double>(time_upper_bound)/MEASUREMENTS);
  os_plot << std::endl;

  }


  return 0;
}
