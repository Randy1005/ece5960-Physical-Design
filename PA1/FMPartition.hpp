#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>


namespace FMPartition {

const int64_t pmax = 15;

struct Cell;
struct Net;
struct GainBucketNode;
struct GainBucketList;

class FMPartition {
public:
  FMPartition();
 
  // for convenience, I also get the cell count while reading from file
  void read_netlist_file(const std::string& inputFileName);

  void init();

  void dump_nets();

  // creates an initial partition for F-M to improve
  void init_partition();

  // initialize bucket gain list
  void init_gainbucket();

  // checks if moving a cell respects the balance criterion
  bool is_move_balanced(int64_t cell_id);

  // cut size
  int64_t calc_cut();

  std::vector<Net> nets;
  std::vector<Cell> cells;
  
  std::vector<GainBucketList> gain_bucket;

  std::unordered_map<int64_t, std::vector<int64_t>> cell_to_nets;
  std::unordered_map<int64_t, std::vector<int64_t>> net_to_cells;

  double min_balance, max_balance;

  double balance_factor;
  int64_t curr_max_gain = 0;
  int64_t cell_count = 0, net_count = 0;
};


struct Net {
  Net();
  Net(int64_t id);
  int64_t id;
  
  bool is_cut;
  void update_is_cut(FMPartition& fm);
};

struct Cell {
  Cell();
  Cell(int64_t id);
  
  int64_t fs(FMPartition& fm);
  int64_t te(FMPartition& fm);
  void update_gain(FMPartition& fm);

  int64_t id;
  // cache its gain value
  int64_t gain;
  int partition_id;
};

struct GainBucketNode {
  int64_t cell_id;
  GainBucketNode(int64_t cell_id);
  GainBucketNode* prev, *next;  
};

struct GainBucketList {

  GainBucketNode* head, *tail;  
  GainBucketList();

  ~GainBucketList();


  // inserts a new node from the back
  void insert_back(int64_t cell_id);

  // move an allocated node to the back
  void move_to_back(GainBucketNode**);

  // pops the first node from the list
  GainBucketNode* pop_front();

  // remove a node from list (by id), get the ref to it
  GainBucketNode* remove(int64_t cell_id);

  // dump info for debugging
  void dump(std::ostream& os) const;

  
};



}
