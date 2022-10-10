#include <iostream>
#include <floorplanner/floorplanner.hpp>
#include <chrono>

int main(int argc, char* argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: ./fp [alpha] [input.block] [input.net] [output.rpt]" << std::endl;
    std::exit(EXIT_FAILURE);
  }
 
  
  std::chrono::steady_clock::time_point start_time, end_time; 

  start_time = std::chrono::steady_clock::now(); 
  
  
  std::cout << "start floor planning ...\n";
  floorplanner::FloorPlanner fp;
  fp.read_input(argv[1], argv[2], argv[3]);
  fp.init_floorplan();
  end_time = std::chrono::steady_clock::now(); 


   
  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
            << elapsed_time.count()
            << " ms\n";
  

  return 0;
}
