#include <fstream>
#include <cstdlib>
#include <ctime>
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


void FMPartition::dump_nets() {
  for (const auto& e : cell_to_nets) {
    std::cout << "Cell " << e.first << " | Partition: " << cells[e.first].partition_id << "| nets: ";
    for (auto& n : e.second) {
      std::cout << "[" << n << "|" << nets[n].is_cut << "]";
    }
    std::cout << "\n";
  }

  
  for (auto& e : net_to_cells) {
    std::cout << "Net " << e.first << " | cells: ";
    for (auto& n : e.second) {
      std::cout << n << " ";
    }
    std::cout << "\n";
  }
  
  
  std::cout << "balance_factor: " << balance_factor << "\n";
  std::cout << "min_balance: " << min_balance << " ,max_balance: " << max_balance << "\n";
  std::cout << "cell_count: " << cell_count << "\n";
  std::cout << "net_count: " << net_count << "\n";
}

}
