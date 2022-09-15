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
  fm.init();
  fm.init_partition();
  fm.init_gainbucket();

  end_time = std::chrono::steady_clock::now(); 

  // fm.dump_nets();
  std::cout << "Cut size: " << fm.calc_cut() << "\n";

  /*
  std::vector<FMPartition::GainBucketList> g;
  g.resize(4);

  g[0].insert_back(24);
  g[0].insert_back(10);
  g[0].insert_back(12);
  g[0].insert_back(1);
  g[0].insert_back(80);

  for (int i = 0; i < g.size(); i++) {
    std::cout << "bucket " << i << ":";
    g[i].dump(std::cout);
    std::cout << "\n";
  }

  g[0].remove(12);
  g[0].remove(1);
  g[0].remove(12314);
  g[0].insert_back(12492);
  g[0].insert_back(18124);
  g[0].remove(24);
  g[0].insert_back(12599);
  FMPartition::GainBucketNode* n0 = g[0].remove(10);

  for (int i = 0; i < g.size(); i++) {
    std::cout << "bucket " << i << ":";
    g[i].dump(std::cout);
    std::cout << "\n";
  }

  g[1].remove(12);
  g[1].move_to_back(&n0);
  FMPartition::GainBucketNode* n1 = g[0].pop_front();
  g[2].move_to_back(&n1);
  FMPartition::GainBucketNode* n2 = g[1].pop_front();
  g[3].move_to_back(&n2);

  for (int i = 0; i < g.size(); i++) {
    std::cout << "bucket " << i << ":";
    g[i].dump(std::cout);
    std::cout << "\n";
  }
  */

  // print bucket
  for (int64_t i = 0; i < fm.gain_bucket.size(); i++) {
    std::cout << "bucket " << i << ": ";
    fm.gain_bucket[i].dump(std::cout);
    std::cout << "\n";
  }

  /*
  for (int64_t i = 0; i < fm.cells.size(); i++) {
    std::cout << "Gain " << i << ": " << fm.cells[i].gain;
  }
  */

  std::cout << fm.curr_max_gain << "\n";



  std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time;  
  std::cout << "Run time: " 
    << elapsed_time.count()
    << " ms\n";


  return 0;



}
