#include "FMPartition.hpp"
#include <chrono>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ./exec [input_file] [output_file]" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  FMPartition::FMPartition fm;

  std::chrono::steady_clock::time_point start_time, end_time; 
  start_time = std::chrono::steady_clock::now(); 

  fm.read_netlist_file(argv[1]); 
  int cut = fm.fm_full_pass();
  end_time = std::chrono::steady_clock::now(); 
  
  std::cout << "cut size: " << cut << "\n";
  fm.write_result(argv[2]);

  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
    << elapsed_time.count()
    << " ms\n";

  return 0;
}
