#include <iostream>
#include <floorplanner/floorplanner.hpp>
#include <chrono>
#include <fstream>

int main(int argc, char* argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: ./fp [alpha] [input.block] [input.net] [output.rpt]" << std::endl;
    std::exit(EXIT_FAILURE);
  }
	floorplanner::FloorPlanner fp;

  std::chrono::steady_clock::time_point start_time, end_time; 
  start_time = std::chrono::steady_clock::now(); 
  fp.read_input(argv[1], argv[2], argv[3]);
  fp.init_floorplan();
	fp.sa_multiple_passes();
  end_time = std::chrono::steady_clock::now(); 
	
	fp.dump(std::cout);
	std::ofstream ofs(argv[4]);

  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
            << elapsed_time.count()
            << " ms\n";
  
  fp.write_result(ofs, elapsed_time.count() / 1000.0);

  return 0;
}
