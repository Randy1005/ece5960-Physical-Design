#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cassert>
#include "FMPartition.hpp"


namespace FMPartition {

Net::Net() :
  id(0)
{

}


Net::Net(int id) :
  id(id)
{
  
}

// updates cut/uncut
void Net::update_is_cut(FMPartition& fm) {
  // TODO:
  // maintain a cell count for both sides
  // for each net
  // if both of the count != 0
  // then this net is cut

  // if any one of the cells belongs to another partition
  // this net is considered cut
  auto& cell_ids = fm.net_to_cells[id];
  
  for (int i = 1; i < cell_ids.size(); i++) {
    if (fm.cells[cell_ids[i]].partition_id ^ fm.cells[cell_ids[0]].partition_id) {
      is_cut = true;
      return;
    }   
  }
  
  is_cut = false;

}


Cell::Cell() :
  id(0)
{

}

Cell::Cell(int id) :
  id(id),
  locked(0)
{

}

int Cell::fs(FMPartition& fm) {
  int fs = 0;

  auto& ns = fm.cell_to_nets[id];
  
  // visit each associated net to this cell
  for (int net : ns) {
    // is this net cut?
    if (!fm.nets[net].is_cut) {
      continue;
    }

    // is this net connected to another cell in
    // the same partition as this cell?
    auto& cs = fm.net_to_cells[fm.nets[net].id];
    bool net_connected_to_multcells = false;
    for (int cell : cs) {
      if (cell != id && fm.cells[cell].partition_id == partition_id) {
        net_connected_to_multcells = true;
        break;
      }
    }

    if (net_connected_to_multcells) {
      continue;
    }
    else {
      // we found a cut net
      // that only connects to this cell 
      // [in the same partition]
      fs++;
    }
  }

  return fs;

}

int Cell::te(FMPartition& fm) {
  // simply uncut nets connected to this cell
  int te = 0;
  
  auto& ns = fm.cell_to_nets[id];
  for (auto& net : ns) {
    if (!fm.nets[net].is_cut) {
      te++;
    }
  }

  return te;
}

void Cell::update_gain(FMPartition& fm) {

}

GainBucketNode::GainBucketNode(int cell_id) :
  cell_id(cell_id),
  next(nullptr),
  prev(nullptr)
{

}

GainBucketList::GainBucketList() :
    head(nullptr),
    tail(nullptr)
{

}

GainBucketList::~GainBucketList() {

}

void GainBucketList::insert_back(int cell_id) {
  GainBucketNode* n = new GainBucketNode(cell_id);
  
  if (tail == nullptr) {
    tail = head = n;
    return;
  }
  
  
  if (tail->next == nullptr) {
    tail->next = n;
    n->prev = tail;
    tail = n;
  }

}

GainBucketNode* GainBucketList::remove(int cell_id) { 
  GainBucketNode* curr = head;

  while (curr != nullptr) {
    if (curr->cell_id == cell_id) {
      if (curr == head) {
        head = curr->next;
      }
      else if (curr == tail) {
        tail = curr->prev;
        curr->prev->next = nullptr;
      }
      else {
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
      }


      curr->prev = nullptr;
      curr->next = nullptr;

      return curr;
    }
    curr = curr->next;
  }

  std::cerr << "no node with cell_id: " << cell_id << " exists.\n";
  return nullptr;
}

void GainBucketList::move_to_back(GainBucketNode** n) {
  if (tail == nullptr || head == nullptr) {
    tail = head = *n;
    return;
  }
  
  
  if (tail->next == nullptr) {
    tail->next = *n;
    (*n)->prev = tail;
    tail = *n;
  }
}

GainBucketNode* GainBucketList::pop_front() {
  GainBucketNode* tmp = head;
  head = tmp->next;

  tmp->next = nullptr;
  tmp->prev = nullptr;
  return tmp;
}

void GainBucketList::dump(std::ostream& os) const {
  GainBucketNode* curr = head;
  while (curr != nullptr) {
    os << curr->cell_id << " ";
    curr = curr->next;
  }
  os << "\n";
}




FMPartition::FMPartition() {

}

void FMPartition::read_netlist_file(const std::string& inputFileName) {
  std::ifstream ifs;
  ifs.open(inputFileName);
  
  if (!ifs) {
    throw std::runtime_error("failed to open this file.");
  }

  // start reading in the netlists
  std::string buffer;
  
  // read in the first line: balance factor
  std::getline(ifs, buffer);
  balance_factor = std::stod(buffer);

  // std::cout << "balance factor: " << balance_factor << "\n";

  while (1) {
    ifs >> buffer;
    if (ifs.eof()) {
      break;
    }

    if (buffer == "NET") {
      while (buffer != ";") {
        // first string is net id, e.g. n1, n13 etc.
        ifs >> buffer;
        if (buffer[0] == 'c') {
          int cell = stoul(buffer.substr(1));
          if (cell > cell_count) {
            cell_count = cell;
          }
          cell_to_nets[cell-1].push_back(net_count);
          net_to_cells[net_count].push_back(cell-1);
        }
      }
      net_count++;

    }
  }


}

void FMPartition::init() {
  std::srand(std::time(nullptr));
  
  if (cell_count == 0) {
    std::cerr << "cell_count not initialized yet.";
  }
  else {
      
    cells.resize(cell_count);
    for (int i = 0; i < cell_count; i++) {
      cells[i] = Cell(i);
    }

    nets.resize(net_count);
    for (int i = 0; i < net_count; i++) {
      nets[i] = Net(i);
    }

    // calculate balance criterion
    min_balance = cell_count * (1.0f - balance_factor) / 2.0f;
    max_balance = cell_count * (1.0f + balance_factor) / 2.0f;
  }
}

void FMPartition::init_partition() {
  // to satisfy the balance constraint
  // I simply assign the first half to one partition
  // and the other half to the other
  for (int i = 0; i < (cell_count / 2); i++) {
    cells[i].partition_id = 0;
  }

  for (int i = (cell_count / 2); i < cell_count; i++) {
    cells[i].partition_id = 1;
  }

  part0_cell_count = cell_count / 2;
  part1_cell_count = cell_count - part0_cell_count;

  // update cut/uncut
  for (int i = 0; i < net_count; i++) {
    nets[i].update_is_cut(*this);
  }

}

int FMPartition::calc_cut() {
  int cut = 0;
  for (auto& n : nets) {
    if (n.is_cut) {
      cut++;
    }    
  }

  return cut;
}

int FMPartition::fm_pass() {
  int locked_cell_cnt = 0;


  int max_gain_seq = 0;
  int max_accu_gain = 0;
  int curr_accu_gain = 0;
  
  // copy cells to a tmp container
  std::vector<Cell> tmp_cells = cells;
  
  while (locked_cell_cnt < cell_count) {
    // navigate the the max gain bucket
    int max_gain_bucket_index = 0;

    bool base_cell_found = false;
    GainBucketNode* node;
    while (!base_cell_found) {
      if (!gain_bucket[max_gain_bucket_index].head) {
        max_gain_bucket_index++;
        continue;
      }
      // found a max gain bucket
      // see if there's a cell that fits the balance
      // criterion
      node = gain_bucket[max_gain_bucket_index].head;
      do {
        if (is_move_balanced(node->cell_id)) {
          // remove this node from the bucket
          gain_bucket[max_gain_bucket_index].remove(node->cell_id);
          base_cell_found = true;
          break;
        }
          
        // std::cout << node->cell_id << "\n";
        node = node->next;
      } while (node != nullptr);
      
      max_gain_bucket_index++;
    }
    

    // record the move order 
    // and gain
    move_order.push_back(node->cell_id);
    if (cells[node->cell_id].gain < 0) {
      curr_accu_gain -= std::abs(cells[node->cell_id].gain);
    } else {
      if (curr_accu_gain + cells[node->cell_id].gain > max_accu_gain) {
        max_accu_gain = curr_accu_gain + cells[node->cell_id].gain;
        max_gain_seq = locked_cell_cnt;
      }
      curr_accu_gain += cells[node->cell_id].gain;
    }

    // lock this cell
    cells[node->cell_id].locked = true;
    locked_cell_cnt++;
    

    // calculate F(net) and T(net)
    // before-move and after-move
    // to identify critical nets
    bool from_part = cells[node->cell_id].partition_id;
    bool to_part = !from_part;
    
    auto& ns = cell_to_nets[node->cell_id];
    for (auto& n : ns) {
      // in to_partition, how many cells
      // are connected to net n?
      auto& cs = net_to_cells[n];
      int T_n = 0;
      for (auto& c : cs) {
        if (cells[c].partition_id == to_part) {
          T_n++;
          if (T_n > 1) {
            break;
          }
        }
      }

      if (T_n > 1) {
        continue;
      }

      // if T(net) == 0
      // increment gains of all free cells
      // connected to net n
      //
      // if T(net) == 1
      // only decrement that one cell's gain
      // and only if it's free
      
      if (T_n == 0) {
        for (auto& c : cs) {
          if (!cells[c].locked) {
            // move to its corresponding bucket
            GainBucketNode* n = gain_bucket[pmax - cells[c].gain].remove(c);
            cells[c].gain++;
            gain_bucket[pmax - cells[c].gain].move_to_back(&n);
          }
        }
      } else if (T_n == 1) {
        for (auto& c : cs) {
          if (cells[c].partition_id == to_part && !cells[c].locked) {
            // move to its corresponding bucket
            GainBucketNode* n = gain_bucket[pmax - cells[c].gain].remove(c);
            cells[c].gain--;
            gain_bucket[pmax - cells[c].gain].move_to_back(&n);
            break;
          }
        }
      }

      // derive F(net) from T(net)
      // F(net) = cell_connected_to_net - T(net)
      int F_n = cs.size() - T_n;
      
      // change net distribution to reflect the move
      F_n--;
      T_n++;
      
      
      if (F_n > 1) {
        continue;
      }

      // if F(net) == 0
      // decrement gains of all free cells
      // connected to net n
      //
      // if F(net) == 1
      // only increment that one cell's gain
      // and only if it's free
      if (F_n == 0) {
        for (auto& c : cs) {
          if (!cells[c].locked) {
            // move to its corresponding bucket
            GainBucketNode* n = gain_bucket[pmax - cells[c].gain].remove(c);
            cells[c].gain--;
            gain_bucket[pmax - cells[c].gain].move_to_back(&n);
          }
        }
      } else if (F_n == 1) {
        for (auto& c : cs) {
          if (cells[c].partition_id == from_part && !cells[c].locked) {
            // move to its corresponding bucket
            GainBucketNode* n = gain_bucket[pmax - cells[c].gain].remove(c);
            cells[c].gain++;
            gain_bucket[pmax - cells[c].gain].move_to_back(&n);
            break;
          }
        }
      }

    } 

    cells[node->cell_id].partition_id = !cells[node->cell_id].partition_id;
  } 



  /*
  std::cout << "best move: " << max_gain_seq << "\n";
  for (int i = 0; i < move_order.size(); i++) {
    std::cout << move_order[i] << ", ";
  }
  std::cout << "\n";
  */


  // get the best move sequence
  // now make the actual moves
  cells = tmp_cells;
  part0_cell_count = cell_count / 2;
  part1_cell_count = cell_count - part0_cell_count;

  for (int i = 0; i < max_gain_seq; i++) {
    int order = move_order[i];
    if (!cells[order].partition_id) {
      part0_cell_count--;
      part1_cell_count++;
    } else {
      part1_cell_count--;
      part0_cell_count++;
    }
    cells[order].partition_id = !cells[order].partition_id;
  }

  // update uncut/cut for nets
  for (auto&n : nets) {
    n.update_is_cut(*this);
  }
   
  return calc_cut();
}

int FMPartition::fm_full_pass() {
  init();
  init_partition();
  init_gainbucket();
  // commented code overwrites the prev
  // valid solution
  /*
  do {
    // set all cells to free
    for (int i = 0; i < cells.size(); i++) {
      cells[i].locked = false;
      cells[i].gain = 0;
    }

    init_gainbucket();
    last_cut = cut_size;
    cut_size = fm_pass();
  } while (cut_size < last_cut);
  */
  
  return fm_pass();
}

void FMPartition::write_result(const std::string& output_file) {
  std::ofstream ofs;
  ofs.open(output_file);
  
  int cut_size = calc_cut(); 
  ofs << "Cutsize = " << cut_size << "\n";
 
  std::cout << part0_cell_count << "\n";
  std::cout << part1_cell_count << "\n";

  std::vector<int> g0, g1;
  int g0_cnt = 0, g1_cnt = 0; 
  for (int i = 0; i < cells.size(); i++) {
    if (!cells[i].partition_id) {
      g0.push_back(cells[i].id + 1);
    } else {
      g1.push_back(cells[i].id + 1);
    }
  }

  ofs << "G1 " << g0.size() << "\n";
  for (int i = 0; i < g0.size(); i++) {
    ofs << "c" << g0[i] << " ";
  }
  ofs << ";\n";
  ofs << "G2 " << g1.size() << "\n";

  for (int i = 0; i < g1.size(); i++) {
    ofs << "c" << g1[i] << " ";
  }
  ofs << ";\n";
}

bool FMPartition::is_move_balanced(int cell_id) {
  if (!cells[cell_id].partition_id) {
    // meaning we're moving it to partition block 1
    int part0 = part0_cell_count - 1;
    int part1 = part1_cell_count + 1;
    
    if (part0 < min_balance || part1 > max_balance) {
      return false;
    } else {
      // std::cout << "moving to block 1\n";
      // std::cout << "G1: " << part0_cell_count << "| G2: " << part1_cell_count << "\n";

      part0_cell_count--;
      part1_cell_count++;
      return true;
    }
  }
  else {
    // we're moving it to partition block 0
    int part0 = part0_cell_count + 1;
    int part1 = part1_cell_count - 1;
    if (part1 < min_balance || part0 > max_balance) {
      return false;
    }
    else {
      // std::cout << "moving to block 0\n";
      // std::cout << "G1: " << part0_cell_count << "| G2: " << part1_cell_count << "\n";
      part0_cell_count++;
      part1_cell_count--;
      return true;
    }
  }

}


void FMPartition::init_gainbucket() {
 
  gain_bucket.clear();
  // TODO: for now pmax = 15 
  // but in class I recall another pmax mentioned
  // gain_bucket.resize(2 * net_count + 1);
  gain_bucket.resize(2 * pmax + 1);
  
  // populate the initial gain bucket list
  for (auto& c : cells) {
    
    int gain = c.fs(*this) - c.te(*this);
    c.gain = gain;
    curr_max_gain = std::max(gain, curr_max_gain);
     
    // map this gain to the gain bucket index
    // positive gain: bucket index = pmax - gain
    // negative gain: bucket index = (2 * pmax + 1) - abs(gain)
    gain_bucket[pmax - gain].insert_back(c.id);
  }
}

void FMPartition::dump_nets() {
  /* 
  for (const auto& e : cell_to_nets) {
    std::cout << "Cell " << e.first << " | Partition: " << cells[e.first].partition_id << "| nets: ";
    for (auto& n : e.second) {
      std::cout << "[" << n << "|" << nets[n].is_cut << "]" << "\t";
    }
    std::cout << "\n";
  }
  */
  

  /*
  for (auto& e : net_to_cells) {
    std::cout << "Net " << e.first << " | cells: ";
    for (auto& n : e.second) {
      std::cout << n << " ";
    }
    std::cout << "\n";
  }
  */
  
  /*
  for (auto& c : cells) {
    std::cout << "cell " << c.id << ": " << c.te(*this) << "\n";
  }
  */
 
   
  
  // std::cout << "balance_factor: " << balance_factor << "\n";
  std::cout << "min_balance: " << min_balance << " ,max_balance: " << max_balance << "\n";
  std::cout << "cell_count: " << cell_count << "\n";
  std::cout << "net_count: " << net_count << "\n";
}

}
