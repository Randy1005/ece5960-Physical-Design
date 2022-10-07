add_library(floorplanner floorplanner.cpp)

set_property(TARGET floorplanner PROPERTY CXX_STANDARD 17)
target_compile_options(floorplanner INTERFACE -Wall -Wextra -Wfatal-errors)
