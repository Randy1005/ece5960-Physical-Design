#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace FMPartition {

struct Cell;
struct Net;
class FMPartition {
public:
  FMPartition();
 
  // for convenience, I also get the cell count while reading from file
  void read_netlist_file(const std::string& inputFileName);

  void init();

  void dump_nets();

  // creates an initial partition for F-M to improve
  void init_partition();

  // checks if moving a cell respects the balance criterion
  bool is_move_balanced(uint64_t cell_id);

  std::vector<Net> nets;
  std::vector<Cell> cells;
  std::unordered_map<uint64_t, std::vector<uint64_t>> cell_to_nets;
  std::unordered_map<uint64_t, std::vector<uint64_t>> net_to_cells;

  double min_balance, max_balance;

  double balance_factor;
  uint64_t cell_count = 0, net_count = 0;
};


struct Net {
  Net();
  Net(uint64_t id);
  uint64_t id;
  
  bool is_cut;
  void update_is_cut(FMPartition& fm);
};

struct Cell {
  Cell();
  Cell(uint64_t id);
  
  uint64_t id;
  int partition_id;
};





}
