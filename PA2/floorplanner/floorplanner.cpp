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


FloorPlanner::FloorPlanner() :
	_rng(_rd())
{
	_uni_real_dist = std::uniform_real_distribution<double>(0, 1);
}


void FloorPlanner::read_input(const std::string& a, const std::string& blk_file, const std::string& net_file) {
  // read in alpha factor
  alpha = std::stod(a); 
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
                        0, 0);
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



	_uni_int_dist = std::uniform_int_distribution(0, n_blks - 1);
	ifs.close();
}



void FloorPlanner::init_floorplan() {
  _pos_seq_pair.resize(n_blks);
	_neg_seq_pair.resize(n_blks);
	_match.resize(n_blks);
	_match_x_rev.resize(n_blks);

	// initial sequence pairs
	// just line the blocks up from left to right
	for (int i = 0; i < n_blks; i++) {
		_pos_seq_pair[i] = _neg_seq_pair[i] = i;
	}

	// initialize Anorm, Wnorm
	_w_norm = 1.0;
	_area_norm = 1.0;

	// update the match list
	// and recover floorplan from sequence pairs
	_update_match();
	_update_weighted_lcs();	
}

void FloorPlanner::simulated_annealing() {
	int accept_moves = 1;
	int accum_area = _curr_bbox_w * _curr_bbox_h;
	int accum_w = hpwl(); 

	double temperature = 100.0;
	bool frozen = false;

	while (!frozen) {
		// cache previous sequence pair
		// so we could undo
		std::vector<int> old_pos_seq = _pos_seq_pair;
		std::vector<int> old_neg_seq = _neg_seq_pair;
		std::vector<Match> old_match = _match;
		std::vector<Match> old_match_x_rev = _match_x_rev;

		
		/*
		std::cout << "delta = " << delta << "\n";
		if (delta < 0) {
			std::cout << "must accept\n";
			accept_moves++;
			accum_area += _curr_bbox_w * _curr_bbox_h;
			accum_w += hpwl();
			_area_norm = accum_area / accept_moves;
			_w_norm = accum_w / accept_moves;
		}
		else {
			double uni_rand = _uni_real_dist(_rng);
			std::cout << "uni rand = " << uni_rand << "\n";
			std::cout << "exp = " << std::exp(-delta / temperature) << "\n";
			if (uni_rand < std::exp(-delta / temperature)) {
			}
			else {
				// undo the move we just did
				_pos_seq_pair = old_pos_seq;
				_neg_seq_pair = old_neg_seq;
				_match = old_match;
				_match_x_rev = old_match_x_rev;
			}
		}
		*/


		if (temperature < 10.0) {
			frozen = true;
		}
		temperature *= .95;
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
	/*
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
	*/

	std::cout << "current hpwl = " << hpwl() << "\n";
	std::cout << "current cost = " << cost() << "\n";
}


void FloorPlanner::_update_match() {
	for (int i = 0; i < n_blks; i++) {
		_match[i].at_x = _pos_seq_pair[i];
		_match_x_rev[i].at_x = n_blks - _match[i].at_x - 1;
	}
}

void FloorPlanner::_update_weighted_lcs() {
	std::vector<int> pos;
	pos.resize(n_blks);
	// assign position to floorplan
	_curr_bbox_w = weighted_lcs(_pos_seq_pair, _neg_seq_pair,
			pos, _match, 1);
	
	std::vector<int> pos_seq_rev = _pos_seq_pair;
	std::reverse(pos_seq_rev.begin(), pos_seq_rev.end());
	
	_curr_bbox_h = weighted_lcs(pos_seq_rev, _neg_seq_pair,
			pos, _match_x_rev, 0);
}

void FloorPlanner::_swap_blks_pos() {
	int blk_a = _uni_int_dist(_rng);
	int blk_b = _uni_int_dist(_rng);
	

	std::cout << "blk_a = " << blk_a << "\n";
	std::cout << "blk_b = " << blk_b << "\n";

	// swap blk_a and blk_b
	std::swap(_pos_seq_pair[blk_a], _pos_seq_pair[blk_b]);

	std::cout << "after swap, pos_seq = \n";
	for (int i = 0; i < _pos_seq_pair.size(); i++) {
		std::cout << _pos_seq_pair[i] << ", ";
	}

	// update match
	_match[_pos_seq_pair[blk_b]].at_x = blk_b;
	_match[_pos_seq_pair[blk_a]].at_x = blk_a;

	// FIXME: update match with seq_x reversed
}


}
