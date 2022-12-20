#include <iostream>
#include <router/router.hpp>
#include <chrono>
#include <fstream>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ./Router [input] [output]" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  router::Router rt; 

  std::chrono::steady_clock::time_point start_time, end_time; 
  start_time = std::chrono::steady_clock::now(); 
  rt.read_input(argv[1]);
  rt.build_adj_list();
  rt.route();
  end_time = std::chrono::steady_clock::now(); 

  rt.dump(std::cout);


  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
            << elapsed_time.count()
            << " ms\n";
  

  return 0;
}
