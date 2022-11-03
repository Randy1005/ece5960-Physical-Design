#include <iostream>
#include <floorplanner/floorplanner.hpp>
#include <chrono>

int main(int argc, char* argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: ./fp [alpha] [input.block] [input.net] [output.rpt]" << std::endl;
    std::exit(EXIT_FAILURE);
  }
	floorplanner::FloorPlanner fp;

	/*
	std::vector<int> seq_x{3, 2, 0, 5, 1, 4};
	std::vector<int> seq_y{5, 2, 4, 3, 0, 1};
	std::vector<int> seq_x_R = seq_x;
	std::reverse(seq_x_R.begin(), seq_x_R.end());
	
	std::vector<floorplanner::Match> match, match_xR;
	match.push_back({2, 4});
	match.push_back({4, 5});
	match.push_back({1, 1});
	match.push_back({0, 3});
	match.push_back({5, 2});
	match.push_back({3, 0});
	
	match_xR.push_back({3, 4});
	match_xR.push_back({1, 5});
	match_xR.push_back({4, 1});
	match_xR.push_back({5, 3});
	match_xR.push_back({0, 3});
	match_xR.push_back({2, 0});

	std::vector<int> pos(seq_x.size(), -1);
	
  fp.read_input(argv[1], argv[2], argv[3]);
	int l = fp.weighted_lcs(seq_x, seq_y, pos, match, 1);
	std::cout << "lcs length: " << l << "\n";
	std::cout << "x coords:\n";
	for (int i = 0; i < pos.size(); i++) {
		std::cout << pos[i] << ", ";
	}
	std::cout << "\n";
	
		
	int l2 = fp.weighted_lcs(seq_x_R, seq_y, pos, match_xR, 0);
	std::cout << "lcs length: " << l2 << "\n";
	std::cout << "y coords:\n";
	for (int i = 0; i < pos.size(); i++) {
		std::cout << pos[i] << ", ";
	}
	std::cout << "\n";
	*/

  std::chrono::steady_clock::time_point start_time, end_time; 
  start_time = std::chrono::steady_clock::now(); 
  
  fp.read_input(argv[1], argv[2], argv[3]);
  fp.init_floorplan();

	fp.simulated_annealing();

	fp.dump(std::cout);
  end_time = std::chrono::steady_clock::now(); 


   
  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
            << elapsed_time.count()
            << " ms\n";
  

  return 0;
}
