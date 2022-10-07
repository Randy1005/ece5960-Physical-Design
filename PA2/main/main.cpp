#include <iostream>
#include <floorplanner/floorplanner.hpp>
#include <chrono>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ./fp input_file output_file" << std::endl;
    std::exit(EXIT_FAILURE);
  }
 
  
  std::chrono::steady_clock::time_point start_time, end_time; 

  start_time = std::chrono::steady_clock::now(); 
  
  
  std::cout << "start floor planning ...\n";


  end_time = std::chrono::steady_clock::now(); 


   
  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
            << elapsed_time.count()
            << " ms\n";
  

  return 0;
}
