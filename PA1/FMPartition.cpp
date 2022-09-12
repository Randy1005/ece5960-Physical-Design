#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "FMPartition.hpp"


namespace FMPartition {

static unsigned long x=123456789, y=362436069, z=521288629;
unsigned long xorshf96(void) {          //period 2^96-1
unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

   t = x;
   x = y;
   y = z;
   z = t ^ x ^ y;

  return z;
}

Net::Net() :
  id(0)
{

}


Net::Net(uint64_t id) :
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
  
  for (uint64_t i = 1; i < cell_ids.size(); i++) {
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

Cell::Cell(uint64_t id) :
  id(id)
{

}

uint64_t Cell::fs(FMPartition& fm) {
  uint64_t fs = 0;

  auto& ns = fm.cell_to_nets[id];
  
  // visit each associated net to this cell
  for (uint64_t net : ns) {
    // is this net cut?
    if (!fm.nets[net].is_cut) {
      continue;
    }

    // is this net connected to another cell in
    // the same partition as this cell?
    auto& cs = fm.net_to_cells[fm.nets[net].id];
    bool net_connected_to_multcells = false;
    for (uint64_t cell : cs) {
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

uint64_t Cell::te(FMPartition& fm) {
  // simply uncut nets connected to this cell
  uint64_t te = 0;
  
  auto& ns = fm.cell_to_nets[id];
  for (auto& net : ns) {
    if (!fm.nets[net].is_cut) {
      te++;
    }
  }

  return te;
}

GainBucket::GainBucket(int64_t cell_id) :
  cell_id(cell_id)
{

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
          uint64_t cell = stoul(buffer.substr(1));
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
    for (uint64_t i = 0; i < cell_count; i++) {
      cells[i] = Cell(i);
    }

    nets.resize(net_count);
    for (uint64_t i = 0; i < net_count; i++) {
      nets[i] = Net(i);
    }

    // calculate balance criterion
    min_balance = cell_count * (1.0f - balance_factor) / 2.0f;
    max_balance = cell_count * (1.0f + balance_factor) / 2.0f;
  }
}

void FMPartition::init_partition() {
  // TODO:
  // random assignment for now
  // how to improve it?
  for (uint64_t i = 0; i < cell_count; i++) {
    cells[i].partition_id = std::rand() & 1;
  }


  // update cut/uncut
  for (uint64_t i = 0; i < net_count; i++) {
    nets[i].update_is_cut(*this);
  }

}

uint64_t FMPartition::calc_cut() {
  uint64_t cut = 0;
  for (auto& n : nets) {
    uint64_t cell_cnt0 = 0, cell_cnt1 = 0;
    
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



void FMPartition::init_gainbucket() {
  // TODO: for now pmax = no. of nets 
  // but in class I recall another pmax mentioned
  for (uint64_t i = 0; i < 2 * net_count + 1; i++) {
    gain_bucket.push_back(new GainBucket(-1));
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
