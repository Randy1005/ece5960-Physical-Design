#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>


namespace FMPartition {

const int pmax = 50;

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
  
  // performs one pass of improvement
  // returns cut size
  int fm_pass();
  
  int fm_full_pass();
  
  void write_result(const std::string& output_file);
  
  // checks if moving a cell respects the balance criterion
  bool is_move_balanced(int cell_id);

  // cut size
  int calc_cut();
  
  std::vector<int> acc_gain;
  std::vector<int> move_order;
  std::vector<Net> nets;
  std::vector<Cell> cells;
  std::vector<GainBucketList> gain_bucket;
  std::unordered_map<int, std::vector<int>> cell_to_nets;
  std::unordered_map<int, std::vector<int>> net_to_cells;

  double min_balance, max_balance;

  double balance_factor;
  int curr_max_gain = 0;
  int cell_count = 0, net_count = 0;
  int part0_cell_count = 0, part1_cell_count = 0; 
};


struct Net {
  Net();
  Net(int id);
  int id;
  
  bool is_cut;
  void update_is_cut(FMPartition& fm);
};

struct Cell {
  Cell();
  Cell(int id);
  
  int fs(FMPartition& fm);
  int te(FMPartition& fm);
  void update_gain(FMPartition& fm);

  int id;
  // cache its gain value
  int gain;
  bool locked;
  bool partition_id;
};

struct GainBucketNode {
  int cell_id;
  GainBucketNode(int cell_id);
  GainBucketNode* prev, *next;  
};

struct GainBucketList {

  GainBucketNode* head, *tail;  
  GainBucketList();

  ~GainBucketList();
  // inserts a new node from the back
  void insert_back(int cell_id);

  // move an allocated node to the back
  void move_to_back(GainBucketNode**);

  // pops the first node from the list
  GainBucketNode* pop_front();

  // remove a node from list (by id), get the ref to it
  GainBucketNode* remove(int cell_id);

  // dump info for debugging
  void dump(std::ostream& os) const;
};

}
