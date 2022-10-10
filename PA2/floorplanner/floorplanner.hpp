#include <string>
#include <vector>
#include <unordered_map>
#include <random>

namespace floorplanner {

struct Block;
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

class FloorPlanner {
public:
  FloorPlanner() = default;
  /**
   * @brief read_input
   * reads input from *.block, *.nets files
   * and user input alpha factor
   */
  void read_input(const std::string& a, const std::string& blk_file, const std::string& net_file); 

  /**
   * @brief init_floorplan
   * creates an initial valid polish expression
   * for simulated annealing procedure
   */
  void init_floorplan();


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
  std::unordered_map<std::string, int> _name_to_macro;
  std::vector<std::vector<int>> _net_to_macros;
  
  // NOTE:
  // currenty both terminals and blocks are stored in _macros
  // so be cautious while floorplanning
  std::vector<Macro> _macros;

  // polish expression
  // for slicing floorplan
  std::vector<std::string> _polish_expr;

  std::default_random_engine _rng;
};



}
