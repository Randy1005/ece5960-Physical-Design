#include "floorplanner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cctype>
#include <locale>
#include <algorithm>
#include <cassert>

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
	_uni_int_dist02 = std::uniform_int_distribution<int>(0, 2);
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
	
		_macros[blk].name = blk_name;
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

		_macros[n_blks + term].name = blk_name;
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

	// update the match list
	// and recover floorplan from sequence pairs
	for (int i = 0; i < n_blks; i++) {
		_match[i].at_x = _match[i].at_y = _match_x_rev[i].at_y = 
			_pos_seq_pair[i];
		_match_x_rev[i].at_x = n_blks - _match[i].at_x - 1;
	}
	
	_update_weighted_lcs();	

	// initialize Anorm, Wnorm
	// _w_norm = hpwl();
	// _area_norm = _curr_bbox_w * _curr_bbox_h;
}

void FloorPlanner::simulated_annealing() {

	double temperature = 100000.0;
	double orig_temp = temperature;
	bool frozen = false;

	while (!frozen) {
		int k = 10;
		while (k--) {
			// cache previous sequence pair
			// so we could undo
			std::vector<int> old_pos_seq = _pos_seq_pair;
			std::vector<int> old_neg_seq = _neg_seq_pair;
			std::vector<Match> old_match = _match;
			std::vector<Match> old_match_x_rev = _match_x_rev;
			std::vector<Macro> old_macros = _macros;
			int old_bbox_w = _curr_bbox_w;
			int old_bbox_h = _curr_bbox_h;
			double old_cost = cost();

			int move_choice = _uni_int_dist02(_rng);

			if (move_choice == 0) {
				_swap_blks_pos();
			}
			else {
				_swap_blks_neg();
			}
			
			_update_weighted_lcs();
			std::cout << "cost = " << cost() << "\n";
			double delta = cost() - old_cost;
			std::cout << "delta = " << delta << "\n";
			// FIXME: scale delta by ?
			
			if (delta < 0) {
				std::cout << "accept\n";
				// accept_moves++;
				// accum_area += _curr_bbox_w * _curr_bbox_h;
				// accum_w += hpwl();
				// _area_norm = accum_area / accept_moves;
				// _w_norm = accum_w / accept_moves;
			}
			else {
				double uni_rand = _uni_real_dist(_rng);
				/*
				double uni_rand = _uni_real_dist(_rng);
				std::cout << "uni_rand = " << uni_rand << "\n";
				double p = std::exp(static_cast<double>(-delta) / temperature);
				std::cout << "exp = " << p << "\n";
				*/
				// if (uni_rand <= .9) {
					// undo the move we just did
					_pos_seq_pair = old_pos_seq;
					_neg_seq_pair = old_neg_seq;
					_match = old_match;
					_match_x_rev = old_match_x_rev;
					_macros = old_macros;
					_curr_bbox_w = old_bbox_w;
					_curr_bbox_h = old_bbox_h;
				// }
										
			
			}

		}

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
	std::cout << "current chip area = " << _curr_bbox_w * _curr_bbox_h << "\n";
	std::cout << "current cost = " << cost() << "\n";

	visualize();
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
	while (blk_b == blk_a) {
		blk_b = _uni_int_dist(_rng);
	}

	// swap blk_a and blk_b
	std::swap(_pos_seq_pair[blk_a], _pos_seq_pair[blk_b]);

	// update match
	_match[_pos_seq_pair[blk_b]].at_x = blk_b;
	_match[_pos_seq_pair[blk_a]].at_x = blk_a;

	int rev_blk_a = n_blks - 1 - blk_a;
	int rev_blk_b = n_blks - 1 - blk_b;
	std::vector<int> _pos_seq_pair_rev = _pos_seq_pair;
	std::reverse(_pos_seq_pair_rev.begin(), _pos_seq_pair_rev.end());
	_match_x_rev[_pos_seq_pair_rev[rev_blk_b]].at_x = rev_blk_b;
	_match_x_rev[_pos_seq_pair_rev[rev_blk_a]].at_x = rev_blk_a;
	
}


void FloorPlanner::_swap_blks_neg() {
	int blk_a = _uni_int_dist(_rng);
	int blk_b = _uni_int_dist(_rng);
	while (blk_b == blk_a) {
		blk_b = _uni_int_dist(_rng);
	}

	// swap blk_a and blk_b
	std::swap(_neg_seq_pair[blk_a], _neg_seq_pair[blk_b]);

	// update match
	_match[_neg_seq_pair[blk_b]].at_y = blk_b;
	_match[_neg_seq_pair[blk_a]].at_y = blk_a;
	
	int rev_blk_a = n_blks - 1 - blk_a;
	int rev_blk_b = n_blks - 1 - blk_b;
	std::vector<int> _neg_seq_pair_rev = _neg_seq_pair;
	std::reverse(_neg_seq_pair_rev.begin(), _neg_seq_pair_rev.end());
	_match_x_rev[_neg_seq_pair_rev[rev_blk_b]].at_y = rev_blk_b;
	_match_x_rev[_neg_seq_pair_rev[rev_blk_a]].at_y = rev_blk_a;
	
}

}
