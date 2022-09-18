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


Net::Net(int64_t id) :
  id(id)
{
  
}

// updates cut/uncut
void Net::update_is_cut(FMPartition& fm) {
  // TODO:
  // could possibly be optimized by maintaining if we moved
  // associated cell or not
  
  // if any one of the cells belongs to another partition
  // this net is considered cut
  auto& cell_ids = fm.net_to_cells[id];
  
  for (int64_t i = 1; i < cell_ids.size(); i++) {
    if (fm.cells[cell_ids[i]].partition_id != fm.cells[cell_ids[0]].partition_id) {
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

Cell::Cell(int64_t id) :
  id(id),
  locked(0)
{

}

int64_t Cell::fs(FMPartition& fm) {
  int64_t fs = 0;

  auto& ns = fm.cell_to_nets[id];
  
  // visit each associated net to this cell
  for (int64_t net : ns) {
    // is this net cut?
    if (!fm.nets[net].is_cut) {
      continue;
    }

    // is this net connected to another cell in
    // the same partition as this cell?
    auto& cs = fm.net_to_cells[fm.nets[net].id];
    bool net_connected_to_multcells = false;
    for (int64_t cell : cs) {
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

int64_t Cell::te(FMPartition& fm) {
  // simply uncut nets connected to this cell
  int64_t te = 0;
  
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

GainBucketNode::GainBucketNode(int64_t cell_id) :
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

void GainBucketList::insert_back(int64_t cell_id) {
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

GainBucketNode* GainBucketList::remove(int64_t cell_id) { 
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
          int64_t cell = stoul(buffer.substr(1));
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
    for (int64_t i = 0; i < cell_count; i++) {
      cells[i] = Cell(i);
    }

    nets.resize(net_count);
    for (int64_t i = 0; i < net_count; i++) {
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
  for (int64_t i = 0; i < (cell_count / 2); i++) {
    cells[i].partition_id = 0;
  }

  for (int64_t i = (cell_count / 2); i < cell_count; i++) {
    cells[i].partition_id = 1;
  }

  part0_cell_count = cell_count / 2;
  part1_cell_count = cell_count - part0_cell_count;


  // update cut/uncut
  for (int64_t i = 0; i < net_count; i++) {
    nets[i].update_is_cut(*this);
  }

}

int64_t FMPartition::calc_cut() {
  int64_t cut = 0;
  for (auto& n : nets) {
    int64_t cell_cnt0 = 0, cell_cnt1 = 0;
    
    auto& cs = net_to_cells[n.id];
    for (auto& c : cs) {
      if (cells[c].partition_id == 0) {
        cell_cnt0++;
      }
    }
    cell_cnt1 = cell_count - cell_cnt0;

    cut += std::min(cell_cnt0, cell_cnt1);
  }

  return cut;
}

int64_t FMPartition::fm_pass() {
  int64_t locked_cell_cnt = 0;


  int64_t max_gain_seq = 0;
  int64_t max_accu_gain = 0;
  int64_t curr_accu_gain = 0;
  
  // copy cells to a tmp container
  std::vector<Cell> tmp_cells = cells;
  
  while (locked_cell_cnt < cell_count) {
    // navigate the the max gain bucket
    int64_t max_gain_bucket_index = 0;

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
      while (!is_move_balanced(node->cell_id)) {
        node = node->next;
      }

      if (!node) {
        // all the cells in this bucket
        // does not fit the balance criterion
        max_gain_bucket_index++;
        continue;
      } else {
         
        // remove this node from the bucket
        if (node == gain_bucket[max_gain_bucket_index].head) {
          gain_bucket[max_gain_bucket_index].pop_front();
        } else {
          if (node == gain_bucket[max_gain_bucket_index].tail) {
            gain_bucket[max_gain_bucket_index].tail = node->prev;
            node->prev->next = nullptr;
          }
          else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
          }
          node->next = node->prev = nullptr;
        }

        base_cell_found = true;
      }
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
      int64_t T_n = 0;
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
    }
    
    // make the move
    cells[node->cell_id].partition_id = !cells[node->cell_id].partition_id;

    // F(net)
    for (auto& n : ns) {
      // in from_partition, how many cells
      // are connected to net n?
      auto& cs = net_to_cells[n];
      int64_t F_n = 0;
      for (auto& c : cs) {
        if (cells[c].partition_id == from_part) {
          F_n++;
          if (F_n > 1) {
            break;
          }
        }
      }

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
   
  }

  // get the best move sequence
  // now let make the actual moves
  cells = tmp_cells;
  for (int64_t i = 0; i < max_gain_seq; i++) {
    int64_t order = move_order[i];
    cells[order].partition_id = !cells[order].partition_id;
  }
   
  return calc_cut();
}

int64_t FMPartition::fm_full_pass() {
  init();
  init_partition();

  int64_t cut_size = calc_cut();
  int64_t last_cut;
  do {
    // set all cells to free
    for (int64_t i = 0; i < cells.size(); i++) {
      cells[i].locked = false;
      cells[i].gain = 0;
    }

    init_gainbucket();
    last_cut = cut_size;
    cut_size = fm_pass();
  } while (cut_size < last_cut);

  return last_cut;
}

void FMPartition::write_result(const std::string& output_file) {
  std::ofstream ofs;
  ofs.open(output_file);
  
  int64_t cut_size = fm_full_pass();
  ofs << "Cutsize = " << cut_size << "\n";
  
  int64_t g1_size = 0, g2_size = 0;
  std::vector<int64_t> g1, g2;
  for (int64_t i = 0; i < cells.size(); i++) {
    if (!cells[i].partition_id) {
      g1_size++;
      g1.push_back(cells[i].id);
    } else {
      g2_size++;
      g2.push_back(cells[i].id);
    }
  }

  ofs << "G1 " << g1_size << "\n";
  for (int64_t i = 0; i < g1_size; i++) {
    ofs << "c" << g1[i] + 1 << " ";
  }
  ofs << ";\n";
  ofs << "G2 " << g2_size << "\n";

  for (int64_t i = 0; i < g2_size; i++) {
    ofs << "c" << g2[i] + 1 << " ";
  }
  ofs << ";\n";
}

bool FMPartition::is_move_balanced(int64_t cell_id) {
  if (cells[cell_id].partition_id == 0) {
    // meaning we're moving it to partition block 1
    int part0 = part0_cell_count - 1;
    int part1 = part1_cell_count + 1;
    if (part0 < min_balance || part1 > max_balance) {
      return false;
    }
  }
  else {
    // we're moving it to partition block 0
    int part0 = part0_cell_count + 1;
    int part1 = part1_cell_count - 1;
    if (part1 < min_balance || part0 > max_balance) {
      return false;
    }
  }

  return true;
}


void FMPartition::init_gainbucket() {
 
  gain_bucket.clear();
  // TODO: for now pmax = 15 
  // but in class I recall another pmax mentioned
  // gain_bucket.resize(2 * net_count + 1);
  gain_bucket.resize(2 * pmax + 1);
  
  // populate the initial gain bucket list
  for (auto& c : cells) {
    
    int64_t gain = c.fs(*this) - c.te(*this);
    c.gain = gain;
    curr_max_gain = std::max(gain, curr_max_gain);
     
    // map this gain to the gain bucket index
    // positive gain: bucket index = pmax - gain
    // negative gain: bucket index = (2 * pmax + 1) - abs(gain)
    gain_bucket[pmax - gain].insert_back(c.id);
  }
}

void FMPartition::dump_nets() {
  
  for (const auto& e : cell_to_nets) {
    std::cout << "Cell " << e.first << " | Partition: " << cells[e.first].partition_id << "| nets: ";
    for (auto& n : e.second) {
      std::cout << "[" << n << "|" << nets[n].is_cut << "]" << "\t";
    }
    std::cout << "\n";
  }
  

  /*
  for (auto& e : net_to_cells) {
    std::cout << "Net " << e.first << " | cells: ";
    for (auto& n : e.second) {
      std::cout << n << " ";
    }
    std::cout << "\n";
  }
  */

  for (auto& c : cells) {
    std::cout << "cell " << c.id << ": " << c.te(*this) << "\n";
  }
  
 
   
  
  // std::cout << "balance_factor: " << balance_factor << "\n";
  std::cout << "min_balance: " << min_balance << " ,max_balance: " << max_balance << "\n";
  std::cout << "cell_count: " << cell_count << "\n";
  std::cout << "net_count: " << net_count << "\n";
}

}
