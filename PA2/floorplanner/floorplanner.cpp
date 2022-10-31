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
    
  } 
  ifs.close();
  
  
  // 2nd file: *.net file 
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

  ifs.close();
}



void FloorPlanner::init_floorplan() {
  _pos_seq_pair.resize(n_blks);
	_neg_seq_pair.resize(n_blks);
	_match.resize(n_blks);
	
	for (int i = 0; i < n_blks; i++) {
		_pos_seq_pair[i] = _neg_seq_pair[i] = i;
		_match[i].at_x = _match[i].at_y = i;
	}
	
		
}


int FloorPlanner::weighted_lcs(const std::vector<int>& seq_x, 
		const std::vector<int>& seq_y,
		std::vector<int>& out_positions,
		std::vector<Match>& match,
		bool is_horizontal) {
	// PRE-CONDITION: match is already updated
	// and out_positions is cleared

	// initialize length with size of n_blks
	std::vector<int> length(n_blks, 0);	
	for (int i = 0; i < n_blks; i++) {
		int block, pos;
		if (is_horizontal) {
			block = seq_x[i];
			pos = match[block].at_y;
		}
		else {
			block = seq_y[i];
			pos = match[block].at_x;
		}
		out_positions[block] = length[pos];
		if (is_horizontal) {
			_macros[block].x = out_positions[block];
		}	
		else {
			_macros[block].y = out_positions[block];
		}
	
		int t;
		if (is_horizontal) {
			t = out_positions[block] + _macros[block].w;
		}
		else {
			t = out_positions[block] + _macros[block].h;
		}

		for (int j = pos; j < n_blks; j++) {
			if (t > length[j]) {
				length[j] = t;
			}
			else {
				break;
			}
		}
	}
	
	return length[n_blks - 1];
}


void FloorPlanner::dump(std::ostream& os) const {
	std::cout << "num nets: " << n_nets << "\n";
	std::cout << "num blks: " << n_blks << "\n";
	
	os << "seq pairs:\n";
	for (int i = 0; i < n_blks; i++) {
		os << _pos_seq_pair[i] << " ";
	}
	os << "\n";

	for (int i = 0; i < n_blks; i++) {
		os << _neg_seq_pair[i] << " ";
	}
	os << "\n";

	std::cout << "current hpwl = " << hpwl() << "\n";

}


void FloorPlanner::_update_match() {
	for (int i = 0; i < n_blks; i++) {
	
	}

}


}
