#include <fstream>
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



Net::Net(uint64_t id) :
  id(id)
{
  
}

// updates cut/uncut simultaneously
bool Net::is_cut() const {
  // TODO:
  // could possibly be optimized by maintaining if we moved
  // associated cell or not
  
  for (uint64_t i = 1; i < cells.size(); i++) {
    // if any one of the cells belongs to another partition
    // this net is considered cut
    // TODO:
    // implement
  
  }

  return false;

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
  uint64_t net_count = 0;

  while (1) {
    ifs >> buffer;
    if (ifs.eof()) {
      break;
    }

    if (buffer == "NET") {
      Net n = Net(net_count++);
      while (buffer != ";") {
        // first string is net id, e.g. n1, n13 etc.
        ifs >> buffer;
        if (buffer[0] == 'c') {
          uint64_t cell_id = stoul(buffer.substr(1));
          if (cell_id > cell_count) {
            cell_count = cell_id;
          }
          cell_to_nets[cell_id-1].push_back(n);
        }

      }

      // read ";", store this net
      nets.push_back(n);
    }
  }


}

void FMPartition::init() {
  if (cell_count == 0) {
    std::cerr << "cell_count not initialized yet.";
  }
  else {
    // initialize FS, TE list
    FS.resize(cell_count);
    TE.resize(cell_count);
    for (uint64_t i = 0; i < cell_count; i++) {
      FS[i] = TE[i] = -1;
    }
  }
}

void FMPartition::init_partition() {
  // TODO:
  // random assignment for now
  // how to improve it?


  // TODO:
  // implement random partition
}


void FMPartition::dump_nets() {
  for (const auto& e : cell_to_nets) {
    std::cout << "Cell " << e.first << "| nets: ";
    for (auto& n : e.second) {
      std::cout << n.id << " ";
    }
    std::cout << "\n";
  }

  std::cout << "cell_count: " << cell_count << "\n";
}

}
