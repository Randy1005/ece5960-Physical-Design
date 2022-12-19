#include "router.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>
#include <limits>

namespace router {

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
			router::Node n;
			n.pin_id = j;
			n.distance = 
				std::pow(pins[i].x - pins[j].x, 2) +
				std::pow(pins[i].y - pins[j].y, 2);
			n.distance = std::sqrt(n.distance);

			adj_list[i].push_back(n);
		}
	}

}

void Router::prim_mst() {
	// initialize weight list & visted & parent 
	weights.resize(num_pins, std::numeric_limits<int>::max());
	parents.resize(num_pins, -1);
	visited.resize(num_pins, false);
	weights[0] = 0;
	
	// get the minimum weight and unvisited vertex
	int min_idx = 0;
	
	int iters = num_pins - 1;
	while (iters--) {
		visited[min_idx] = true;

		// update weight and parent for this
		// minimum weight vertex
		std::vector<router::Node>& adj_verts = adj_list[min_idx];

		for (const auto& n : adj_verts) {
			if (!visited[n.pin_id] && 
					n.distance < weights[n.pin_id]) {
				// update weight and parent
				parents[n.pin_id] = min_idx;
				weights[n.pin_id] = n.distance;
				if (n.distance < weights[min_idx]) {
					min_idx = n.pin_id;
				}
			}
		}
	}

	


}


}
