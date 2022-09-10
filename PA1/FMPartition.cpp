#include <fstream>
#include "FMPartition.hpp"

namespace FMPartition {

Net::Net(uint64_t id) :
  id(id)
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
          n.cells.push_back(Cell(stoi(buffer.substr(1))));
        }
      }

      // read ";", store this net
      nets.push_back(n);
    }
  }

  // put this in a dump method?
  for (const auto& n : nets) {
    for (const auto& c : n.cells) {
      std::cout << c.id << " ";
    }
    std::cout << "\n";
  }


}

}
