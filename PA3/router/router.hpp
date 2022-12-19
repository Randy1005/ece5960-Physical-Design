#include <string>
#include <vector>
#include <iostream>
#include <cmath>

namespace router {

struct Pin;
struct Router;


struct Pin {
	Pin(int x, int y, std::string& name);
	int x;
	int y;
	std::string name;
};

struct Node {
	Node() = default;
	// rectilinear weight
	int distance;

	int pin_id;

};


struct Router {
	Router();
	void read_input(const std::string& input_file);
	void dump(std::ostream& os) const;

	void build_adj_list();

	int llx;
	int lly;
	int urx;
	int ury;
	int num_pins;

	std::vector<router::Pin> pins;
	std::vector<std::vector<router::Node>> adj_list;

};


}
