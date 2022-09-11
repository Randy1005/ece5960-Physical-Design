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

  std::vector<Net> nets;
  std::unordered_map<uint64_t, std::vector<Net>> cell_to_nets;
 
  // TODO:
  // should probably use an additional vector to maintain partition ids
  std::vector<uint64_t> cell_partition_ids;

  std::vector<int64_t> FS;
  std::vector<int64_t> TE;

  double balance_factor;
  uint64_t cell_count = 0;
};


struct Net {
  Net(uint64_t id);
   
  std::vector<Cell> cells;
  uint64_t id;
  

  bool is_cut() const;
};

struct Cell {
  Cell();
  Cell(uint64_t id);
  
  uint64_t id;
};



}
