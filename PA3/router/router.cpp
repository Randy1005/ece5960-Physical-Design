#include "router.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>
#include <limits>
#include <utility>

namespace router {


double dist(Pin& p0, Pin& p1) {
	return std::sqrt(std::pow(p0.x - p1.x, 2) +
									std::pow(p0.y - p1.y, 2));
}


Pin::Pin(int x, int y, std::string& name) :
	x(x),
	y(y),
	name(name) 
{
}


Router::Router() {


}


void Router::read_input(const std::string& input_file) {
	std::ifstream ifs;
	ifs.open(input_file);
	if (!ifs) {
		throw std::runtime_error("failed to open input_file");
	}

	std::string buffer;
	while (true) {
		ifs >> buffer;
		if (ifs.eof()) {
			break;
		}

		if (buffer == "Boundary") {
			ifs >> buffer >> buffer;
			int i = 0;
			for ( ; !isdigit(buffer[i]); i++);
			// buffer is now llx, lly of chip
			if (isdigit(buffer[i])) {
				int val = buffer[i] - '0';
				for (;;) {
					i++;
					if (!isdigit(buffer[i])) {
						llx = val;
						break;
					}
					val *= 10;
					val += buffer[i] - '0';
				}
			}
			
			for ( ; !isdigit(buffer[i]); i++);
			
			if (isdigit(buffer[i])) {
				int val = buffer[i] - '0';
				for (;;) {
					i++;
					if (!isdigit(buffer[i])) {
						lly = val;
						break;
					}
					val *= 10;
					val += buffer[i] - '0';
				}
			}
			
			// now buffer is urx, ury of chip
			ifs >> buffer;

			i = 0;
			for ( ; !isdigit(buffer[i]); i++);
			// buffer is now llx, lly of chip
			if (isdigit(buffer[i])) {
				int val = buffer[i] - '0';
				for (;;) {
					i++;
					if (!isdigit(buffer[i])) {
						urx = val;
						break;
					}
					val *= 10;
					val += buffer[i] - '0';
				}
			}
			

			for ( ; !isdigit(buffer[i]); i++);
			
			if (isdigit(buffer[i])) {
				int val = buffer[i] - '0';
				for (;;) {
					i++;
					if (!isdigit(buffer[i])) {
						ury = val;
						break;
					}
					val *= 10;
					val += buffer[i] - '0';
				}
			}

		}
		else if (buffer == "NumPins") {
			ifs >> buffer >> buffer;
			num_pins = std::stoi(buffer);
		}
		else if (buffer == "PIN") {
			ifs >> buffer;
			std::string pin_name = buffer;

			ifs >> buffer;
			int i = 0;
			int pin_x, pin_y;
			for ( ; !isdigit(buffer[i]); i++);
			// buffer is now x, y of pin
			if (isdigit(buffer[i])) {
				int val = buffer[i] - '0';
				for (;;) {
					i++;
					if (!isdigit(buffer[i])) {
						pin_x = val;
						break;
					}
					val *= 10;
					val += buffer[i] - '0';
				}
			}
			

			for ( ; !isdigit(buffer[i]); i++);
			
			if (isdigit(buffer[i])) {
				int val = buffer[i] - '0';
				for (;;) {
					i++;
					if (!isdigit(buffer[i])) {
						pin_y = val;
						break;
					}
					val *= 10;
					val += buffer[i] - '0';
				}
			}

			// add new pin
			pins.push_back(Pin(pin_x, pin_y, pin_name));
		}
	}
}

void Router::dump(std::ostream& os) const {
	std::cout << "Edges:\n";
	for (auto& e : edges) {
		std::cout << e.first << " - " << e.second << "\n";
	}

	os << "chip llx = " << llx << "\n";
	os << "chip lly = " << lly << "\n";
	os << "chip urx = " << urx << "\n";
	os << "chip ury = " << ury << "\n";
	os << "num pins = " << num_pins << "\n";
}


void Router::build_adj_list() {
	adj_list.resize(num_pins);
	for (int i = 0; i < num_pins; i++) {
		for (int j = 0; j < num_pins; j++) {
			if (j == i) {
				continue;
			}
		
			Node n;
			n.pin_id = j;
			n.distance = dist(pins[i], pins[j]);

			adj_list[i].push_back(n);
		}
	}

}

void Router::prim_mst() {
	// initialize weight list & visted & parent 
	weights.resize(pins.size(), std::numeric_limits<double>::max());
	parents.resize(pins.size(), -1);
	visited.resize(pins.size(), false);
	edges.clear();
	weights[0] = 0.0;
	

	int iters = pins.size() - 1;
	while (iters--) {
		
		// extract vertex with the min weight
		int min_idx;
		double min_weight = std::numeric_limits<double>::max();
		for (int i = 0; i < pins.size(); i++) {
			if (!visited[i] && weights[i] < min_weight) {
				min_idx = i;
				min_weight = weights[i];
			}
		}

		assert(min_idx < pins.size());

		visited[min_idx] = true;

		// update weight and parent for this
		// minimum weight vertex
		assert(min_idx < adj_list.size());
		std::vector<router::Node>& adj_verts = adj_list[min_idx];

		for (const auto& n : adj_verts) {
			if (!visited[n.pin_id] && 
					n.distance < weights[n.pin_id]) {
				// update weight and parent
				parents[n.pin_id] = min_idx;
				weights[n.pin_id] = n.distance;
			}
		}
	}
	// store the edges
	for (int i = 1; i < pins.size(); i++) {
		edges.push_back(std::make_pair(parents[i], i));
	}
}


void Router::route() {
	// compute MST
	prim_mst();

	int cnt = 0;
	std::vector<int> candidates;
	for (int p = 0; p < num_pins; p++) {
		for (int i = 0; i < edges.size(); i++) {
			if (edges[i].first != p && edges[i].second != p) {
				continue;
			}
			else {
				cnt++;
				if (edges[i].first == p) {
					candidates.push_back(edges[i].second);
				} else {
					candidates.push_back(edges[i].first);
				}

				if (cnt == 2) {
				
					int p0 = candidates[0];
					int p2 = candidates[1];
					
					int x0 = pins[p0].x;
					int y0 = pins[p0].y;
					int x1 = pins[p].x;
					int y1 = pins[p].y;
					int x2 = pins[p2].x;
					int y2 = pins[p2].y;
					int x_s = std::max(std::min(x0, x1), 
														std::min(std::max(x0, x1), x2));
					int y_s = std::max(std::min(y0, y1), 
														std::min(std::max(y0, y1), y2));

					std::string name("steiner");
					Pin p_s(x_s, y_s, name);
					if (p_s != pins[p0] && p_s != pins[p] && p_s != pins[p2]) {
						pins.push_back(p_s);
						Node n0, n1;
						n0.pin_id = n1.pin_id = pins.size() - 1;
						n0.distance = dist(p_s, pins[edges[i].first]);
						n1.distance = dist(p_s, pins[edges[i].second]);
						
						adj_list.resize(pins.size());
						adj_list[edges[i].first].push_back(n0);
						adj_list[edges[i].first].push_back(n1);

						Node n2, n3;
						n2.pin_id = p0;
						n3.pin_id = p2;
						n2.distance = n0.distance;
						n3.distance = n1.distance;
						adj_list[n0.pin_id].push_back(n2);
						adj_list[n0.pin_id].push_back(n3);
					}

					candidates.clear();
					cnt = 0;
				}
			}
		}
	}
	
	// reset for another prim
	weights.clear();
	parents.clear();
	visited.clear();
	
	prim_mst();
}

void Router::write_result(std::ostream& os) {
	double wirelength = 0.0;
	for (auto& e : edges) {
		wirelength += dist(pins[e.first], pins[e.second]);
	}

	os << "Wirelength = " << static_cast<long long int>(wirelength) << "\n";
	for (auto& e : edges) {
		if (pins[e.first].x == pins[e.second].x) {
			os << "H-line ";
		}
		else if (pins[e.first].y == pins[e.first].y) {
			os << "V-line ";
		}
		os << "(" << pins[e.first].x << "," << pins[e.first].y
							<< ") (" << pins[e.second].x << "," << pins[e.second].y << ")\n";
	}
}


}
