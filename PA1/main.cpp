#include "FMPartition.hpp"
#include <chrono>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./exec [netlist_file]" << std::endl;
    std::exit(EXIT_FAILURE);

  }

  FMPartition::FMPartition fm;

  std::chrono::steady_clock::time_point start_time, end_time; 
  start_time = std::chrono::steady_clock::now(); 

  fm.read_netlist_file(argv[1]);

  end_time = std::chrono::steady_clock::now(); 



  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
    << elapsed_time.count()
    << " ms\n";


  return 0;



}
