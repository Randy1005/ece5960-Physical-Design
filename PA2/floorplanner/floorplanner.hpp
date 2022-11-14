#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>

namespace floorplanner {

struct Block;
struct Match;
class FloorPlanner;

enum class MacroType {
  BLOCK = 0,
  TERMINAL
};

struct Macro {
  MacroType type;
  int x, y, w, h;
	std::string name;

  Macro() = default;
  Macro(MacroType type, int x, int y, int w, int h):
    type(type), x(x), y(y), w(w), h(h)
  {
  }
};

struct Match {
	int at_x, at_y;
	
	Match() = default;
	Match(int x, int y) :
		at_x(x), at_y(y)
	{
	}
};

class FloorPlanner {
public:
  FloorPlanner();
  /**
   * @brief read_input
   * reads input from *.block, *.nets files
   * and user input alpha factor
   */
  void read_input(const std::string& a, 
			const std::string& blk_file, 
			const std::string& net_file); 

  /**
   * @brief init_floorplan
   * creates an initial positive/negative sequence pair
   * for simulated annealing procedure
   */
  void init_floorplan();

	/**
	 * @brief weighted_lcs
	 * calculates the longest common subsequence(X, Y)
	 * with block dimensions as weights
	 * reference: https://dl.acm.org/doi/pdf/10.1145/343647.343713
	 */
	int weighted_lcs(const std::vector<int>& seq_x, 
			const std::vector<int>& seq_y,
			std::vector<int>& out_position,
			std::vector<Match>& match,
			bool is_horizontal);
	

	/**
	 * @brief simulated_annealing
	 * main method for simulated annealing
	 */
	void simulated_annealing();


	void dump(std::ostream& os) const;

	void write_result(std::ostream& os, double runtime) const;

  inline int id_of(const std::string& name) {
    return _name_to_macro[name];
  }

	inline void sa_multiple_passes() {
		int passes = 0;
		do {
			std::cout << "pass " << passes << "\n";
			simulated_annealing();
			passes++;
		} while ((_curr_bbox_w > chip_width || _curr_bbox_h > chip_height) && passes < 10);
	}
	
	/**
	 * @brief hpwl
	 * calculates the total half-perimeter wire length
	 * for all nets
	 */
	inline int hpwl() const {
		int w = 0;
		int llx, lly = 0;
		for (const auto& net : _net_to_macros) {
			int max_x = 0, min_x = std::numeric_limits<int>::max(); 
			int max_y = 0, min_y = std::numeric_limits<int>::max();
			
			for (const auto& b : net) {
				const Macro& m = _macros[b];

				if (m.type == MacroType::BLOCK) {
					llx =	2 * m.x + (m.w - m.x);	
					lly = 2 * m.y + (m.h - m.y);
				}
				else {
					llx = 2 * m.x;
					lly = 2 * m.y;
				}

				max_x = std::max(llx, max_x);
				min_x = std::min(llx, min_x);
				max_y = std::max(lly, max_y);
				min_y = std::min(lly, min_y);
			}

			w += ((max_x - min_x) + (max_y - min_y));
		}

		return w;
	}

	/**
	 * @brief cost
	 * cost function = alpha * area + (1-alpha) * wirelength
	 */
	inline double cost() const {
		// std::cout << "area = " << _curr_bbox_w * _curr_bbox_h << "\n";
		// std::cout << "w = " << hpwl() << "\n";

		double asp_ratio = static_cast<double>(_curr_bbox_w) / _curr_bbox_h;	
		double curr_area = static_cast<double>(_curr_bbox_w) * _curr_bbox_h;
		double asp_ratio_diff = std::abs(asp_ratio - _outline_asp_ratio); 

	
		return alpha * curr_area + (1 - alpha) * static_cast<double>(hpwl()) + 
					asp_ratio_diff * (curr_area / 2.5); 
	}


	inline void visualize() const {
		std::cout << "{\"block_number\":" << n_blks << ",";
		std::cout << "\"llx\":0,\"lly\":0,"
			<< "\"urx\":" << chip_width / 10.0 << ","
			<< "\"ury\":" << chip_height / 10.0 << ","
			<< "\"area\":" << chip_width * chip_height / 100.0 << ","
			<< "\"coordinates\":[";
		int blks = 0;
		for (blks = 0; blks < n_blks;) {
			std::cout << "{\"idx\":\"" << _macros[blks].name << "\","
				<< "\"llx\":" << _macros[blks].x / 10.0 << ","
				<< "\"lly\":" << _macros[blks].y / 10.0 << ","
				<< "\"width\":" << _macros[blks].w / 10.0 << ","
				<< "\"height\":" << _macros[blks].h / 10.0 << "}";
			if (blks++ < n_blks-1) {
				std::cout << ",";
			}
		}

		std::cout << "]}" << '\n';
	}


  int chip_width;
  int chip_height;
  int n_blks;
  int n_terms;
  int n_nets;
  double alpha;
	double initial_temp;
	int moves_per_temp;

private:
	
	// update the match list
	// and the reversed positive sequence pair 
	void _update_match();

	// update the largest common subsequence
	void _update_weighted_lcs();

	// Move 1: swap 2 blocks in the positive sequence
	void _swap_blks_pos();
	
	// Move 2: swap 2 blocks in the negative sequence
	void _swap_blks_neg();

	// Move 3: change orientation
	void _rotate_blk();
	
  std::unordered_map<std::string, int> _name_to_macro;
  std::vector<std::vector<int>> _net_to_macros;
  
  // NOTE:
  // currenty both terminals and blocks are stored in _macros
  // so be cautious while floorplanning
  std::vector<Macro> _macros;

  // sequence pairs
	std::vector<int> _pos_seq_pair;
	std::vector<int> _neg_seq_pair;

	// current floorplan width, height
	int _curr_bbox_w, _curr_bbox_h;

	// chip aspect ratio
	double _outline_asp_ratio;


  std::default_random_engine _rng;
	std::random_device _rd;
	std::uniform_int_distribution<int> _uni_int_dist;	
	std::uniform_int_distribution<int> _uni_int_dist03;
	std::uniform_real_distribution<double> _uni_real_dist;
	// match list
	// records which index the blocks is at
	// in the positive / negative sequence
	std::vector<Match> _match;

	// match list with x component reversed
	// we need this to calculate vertical lcs
	std::vector<Match> _match_x_rev;

	// area normalize factor
	double _area_norm;

	// wire length normalize factor
	double _w_norm;
};


}
