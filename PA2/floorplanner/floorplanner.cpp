#include "floorplanner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cctype>
#include <locale>
#include <algorithm>

namespace floorplanner {


static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
      return !std::isspace(ch);
  }).base(), s.end());
} 



void FloorPlanner::read_input(const std::string& a, const std::string& blk_file, const std::string& net_file) {
  // read in alpha factor
  alpha = std::stof(a); 
  
  std::ifstream ifs;
  
  
  // 1st file: *.block file 
  ifs.open(blk_file);
  if (!ifs) {
    throw std::runtime_error("failed to open block file.");
  }

  std::string buffer;
  
  // read in the first line: chip outline
  ifs >> buffer >> buffer;
  chip_width = std::stoi(buffer);
  ifs >> buffer;
  
  // trim the trailing whitespace just in case
  // rtrim(buffer);
  chip_height = std::stoi(buffer);
  
  // read in the second line : num blocks
  ifs >> buffer >> buffer;
  n_blks = std::stoi(buffer);

  // read in the third line : num terminals
  ifs >> buffer >> buffer;
  n_terms = std::stoi(buffer);

  _macros.resize(n_blks + n_terms);

  // reserve for polish expression
  _polish_expr.reserve(2 * n_blks + 1);

  std::string blk_name, blk_w, blk_h;
  // read in blocks
  for (int blk = 0; blk < n_blks; blk++) {
    ifs >> blk_name >> blk_w >> blk_h;

    // skip empty line
    if (blk_name.empty() || blk_w.empty() || blk_h.empty()) {
      continue;
    }
    
    _macros[blk] = Macro(MacroType::BLOCK, -1, -1, 
                  std::stoi(blk_w), std::stoi(blk_h));
  
    /*
    std::cout << "blk " << blk << "| type: " << static_cast<int>(_macros[blk].type) << "\n";
    std::cout << "\t" << _macros[blk].x << " | " << _macros[blk].y << "|" <<  _macros[blk].w << "|" << _macros[blk].h << "\n";  
    */

    _name_to_macro.insert(std::make_pair(blk_name, blk));
  }

  std::string blk_x, blk_y;
  // read in terminals
  for (int term = 0; term < n_terms; term++) {
    ifs >> blk_name >> buffer >> blk_x >> blk_y;
    if (blk_name.empty() || blk_x.empty() || blk_y.empty()) {
      continue;
    }

    _macros[n_blks + term] = Macro(MacroType::TERMINAL, stoi(blk_x), stoi(blk_y),
                        -1, -1);
    _name_to_macro.insert(std::make_pair(blk_name, n_blks + term));
    
    /*
    std::cout << "blk " << n_blks + term << "| type: " << static_cast<int>(_macros[n_blks + term].type) << "\n";
    std::cout << "\t" << _macros[n_blks + term].x << " | " << _macros[n_blks + term].y << "|" <<  _macros[n_blks + term].w << "|" << _macros[n_blks + term].h << "\n";  
    */
  } 

  
  ifs.close();
  
  
  // 1st file: *.block file 
  ifs.open(net_file);
  if (!ifs) {
    throw std::runtime_error("failed to open net file.");
  }

  ifs >> buffer >> buffer;
  n_nets = stoi(buffer);
  _net_to_macros.resize(n_nets);

  for (int i = 0; i < n_nets; i++) {
    std::string net_deg;
    ifs >> buffer >> net_deg;
    for (int j = 0; j < stoi(net_deg); j++) {
      ifs >> buffer;
      _net_to_macros[i].push_back(_name_to_macro[buffer]);
    }
  }

  /*
  int t = 33;
  std::cout << "net " << t << ":\n";
  for (int i = 0; i < _net_to_macros[t].size(); i++) {
    std::cout << _net_to_macros[t][i] << " ";
  }
  std::cout << "\n";
  */
  ifs.close();
}



void FloorPlanner::init_floorplan() {
  unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
  _rng = std::default_random_engine(seed);
  std::vector<int> random_blks;
  for (int i = 0; i < n_blks; i++) {
    random_blks.push_back(i);
  }

  std::shuffle(std::begin(random_blks), std::end(random_blks), _rng);
  
  // place the first 2 blocks and their operand (H/V)
  int cnt = 2;
  while (cnt--) {
    _polish_expr.push_back(std::to_string(random_blks[random_blks.size() - 1]));
    random_blks.pop_back();
  }
  _polish_expr.push_back(_rng() % 2 == 0 ? "H" : "V");

  // generates an initial PE that represents
  // a floorplan that packs every block
  // with vertical slices 
  while (!random_blks.empty()) {
    _polish_expr.push_back(std::to_string(random_blks[random_blks.size() - 1]));
    _polish_expr.push_back(_rng() % 2 == 0 ? "H" : "V");
    random_blks.pop_back();
  }


}





}
