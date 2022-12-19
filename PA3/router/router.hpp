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
	void prim_mst();

	int llx;
	int lly;
	int urx;
	int ury;
	int num_pins;

	std::vector<router::Pin> pins;
	std::vector<std::vector<router::Node>> adj_list;


	// to keep track of which vertices are
	// already visited
	// initially all false
	std::vector<bool> visited;

	// weight list to be updated
	// all initialized to INF
	// with one vertex initialized to 0
	// that's the one to start with
	std::vector<int> weights;

	// keep track of parents while we
	// visit each vertex
	// parent[root_vertex] = -1
	std::vector<int> parents;


};


}
