#include "router.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>

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

void Router::dump(std::ostream& os) {
	for (const auto& p : pins) {
		std::cout << "pin name: " << p.name << "\n";
		std::cout << "pin x: " << p.x << "\n";
		std::cout << "pin y: " << p.y << "\n";
		std::cout << "\n";
	}

	std::cout << "chip llx = " << llx << "\n";
	std::cout << "chip lly = " << lly << "\n";
	std::cout << "chip urx = " << urx << "\n";
	std::cout << "chip ury = " << ury << "\n";
	std::cout << "num pins = " << num_pins << "\n";
}



}
