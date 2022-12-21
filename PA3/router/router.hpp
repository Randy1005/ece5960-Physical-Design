#include <string>
#include <vector>
#include <iostream>
#include <cmath>

namespace router {

struct Pin;
struct Router;


struct Pin {
	Pin() = default;
	Pin(int x, int y, std::string& name);
	bool operator==(const Pin& p) {
		return x == p.x && y == p.y;
	}
	
	bool operator!=(const Pin& p) {
		return x != p.x || y != p.y;
	}
	int x;
	int y;
	std::string name;
};

struct Node {
	Node() = default;
	// rectilinear weight
	double distance;

	int pin_id;

};


struct Router {
	Router();
	void read_input(const std::string& input_file);
	void dump(std::ostream& os) const;

	void build_adj_list();
	void prim_mst();
	// TODO:
	// enumerate edges
	// get all the pairs where
	// e1, e2 has one common vertex
	// (meaning it's 3 vertices connected)
	void route();

	void write_result(std::ostream& os);

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
	std::vector<double> weights;

	// keep track of parents while we
	// visit each vertex
	// parent[root_vertex] = -1
	std::vector<int> parents;

	// store the edges
	std::vector<std::pair<int, int>> edges;

	// wirelength savings
	std::vector<double> savings;

	// added steiner points
	std::vector<int> steiner_pts; 
};


}
