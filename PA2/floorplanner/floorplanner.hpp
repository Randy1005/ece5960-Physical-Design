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


  inline int id_of(const std::string& name) {
    return _name_to_macro[name];
  }
	
	/**
	 * @brief hpwl
	 * calculates the total half-perimeter wire length
	 * for all nets
	 */
	inline int hpwl() const {
		int w = 0;
		for (const auto& net : _net_to_macros) {
			int max_x = 0, min_x = std::numeric_limits<int>::max(); 
			int max_y = 0, min_y = std::numeric_limits<int>::max();
			for (const auto& b : net) {
				max_x = std::max(_macros[b].x + _macros[b].w, max_x);
				min_x = std::min(_macros[b].x, min_x);
				max_y = std::max(_macros[b].y + _macros[b].h, max_y);
				min_y = std::min(_macros[b].y, min_y);
			}

			/*
			std::cout << "max_x = " << max_x << "\n";
			std::cout << "min_x = " << min_x << "\n";
			std::cout << "max_y = " << max_y << "\n";
			std::cout << "min_y = " << min_y << "\n";
			*/
			w += ((max_x - min_x) + (max_y - min_y));
		}

		return w;
	}

	/**
	 * @brief cost
	 * cost function = alpha * (Area / Anorm) + (1-alpha) * (WireLength / Wnorm)
	 * where Anorm = average area when we generate a solution m times
	 */
	inline double cost() const {
		std::cout << "area = " << _curr_bbox_w * _curr_bbox_h << "\n";
		std::cout << "w = " << hpwl() << "\n";

		return alpha * ((_curr_bbox_w * _curr_bbox_h) / _area_norm)
			+ (1 - alpha) * (hpwl() / _w_norm);
	}

  int chip_width;
  int chip_height;
  int n_blks;
  int n_terms;
  int n_nets;
  double alpha;

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

  std::default_random_engine _rng;
	std::random_device _rd;
	std::uniform_int_distribution<int> _uni_int_dist;	
	std::uniform_int_distribution<int> _uni_int_dist02;
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
