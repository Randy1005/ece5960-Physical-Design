#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>

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
  FloorPlanner() = default;
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




	void dump(std::ostream& os) const;


  inline int id_of(const std::string& name) {
    return _name_to_macro[name];
  }

  int chip_width;
  int chip_height;
  int n_blks;
  int n_terms;
  int n_nets;
  float alpha;

private:
	
	// update the match list
	void _update_match();

  std::unordered_map<std::string, int> _name_to_macro;
  std::vector<std::vector<int>> _net_to_macros;
  
  // NOTE:
  // currenty both terminals and blocks are stored in _macros
  // so be cautious while floorplanning
  std::vector<Macro> _macros;

  // sequence pairs
	std::vector<int> _pos_seq_pair;
	std::vector<int> _neg_seq_pair;

  std::default_random_engine _rng;
	
	// match list
	// records which index the blocks is at
	// in the positive / negative sequence
	std::vector<Match> _match;
};


}
