#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

namespace FMPartition {

struct Cell;
struct Net;
class FMPartition {
public:
  FMPartition();
  
  void read_netlist_file(const std::string& inputFileName);
  
  std::vector<Net> nets;

  double balance_factor;

};


struct Net {
  Net(uint64_t id);
  
  std::vector<Cell> cells;
  uint64_t id;
  bool get_is_cut();
  // is this net cut/uncut
  bool is_cut;

};

struct Cell {
  Cell(uint64_t id);

  uint64_t id;
  // which partition this cell belongs to
  int partition_id; // for bi-partition, just 0 and 1
};



}
