#include <string>
#include <vector>
#include <iostream>

namespace router {

struct Pin;
struct Router;


struct Pin {
	Pin(int x, int y, std::string& name);
	int x;
	int y;
	std::string name;
};


struct Router {
	Router();
	void read_input(const std::string& input_file);
	void dump(std::ostream& os);

	int llx;
	int lly;
	int urx;
	int ury;
	int num_pins;

	std::vector<router::Pin> pins;
};


}
