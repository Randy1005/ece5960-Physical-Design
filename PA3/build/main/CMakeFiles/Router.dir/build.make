# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/randy/ece5960-Physical-Design/PA3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/randy/ece5960-Physical-Design/PA3/build

# Include any dependencies generated for this target.
include main/CMakeFiles/Router.dir/depend.make

# Include the progress variables for this target.
include main/CMakeFiles/Router.dir/progress.make

# Include the compile flags for this target's objects.
include main/CMakeFiles/Router.dir/flags.make

main/CMakeFiles/Router.dir/main.cpp.o: main/CMakeFiles/Router.dir/flags.make
main/CMakeFiles/Router.dir/main.cpp.o: ../main/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/randy/ece5960-Physical-Design/PA3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object main/CMakeFiles/Router.dir/main.cpp.o"
	cd /home/randy/ece5960-Physical-Design/PA3/build/main && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Router.dir/main.cpp.o -c /home/randy/ece5960-Physical-Design/PA3/main/main.cpp

main/CMakeFiles/Router.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Router.dir/main.cpp.i"
	cd /home/randy/ece5960-Physical-Design/PA3/build/main && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/randy/ece5960-Physical-Design/PA3/main/main.cpp > CMakeFiles/Router.dir/main.cpp.i

main/CMakeFiles/Router.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Router.dir/main.cpp.s"
	cd /home/randy/ece5960-Physical-Design/PA3/build/main && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/randy/ece5960-Physical-Design/PA3/main/main.cpp -o CMakeFiles/Router.dir/main.cpp.s

# Object files for target Router
Router_OBJECTS = \
"CMakeFiles/Router.dir/main.cpp.o"

# External object files for target Router
Router_EXTERNAL_OBJECTS =

main/Router: main/CMakeFiles/Router.dir/main.cpp.o
main/Router: main/CMakeFiles/Router.dir/build.make
main/Router: router/librouter.a
main/Router: main/CMakeFiles/Router.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/randy/ece5960-Physical-Design/PA3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Router"
	cd /home/randy/ece5960-Physical-Design/PA3/build/main && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Router.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
main/CMakeFiles/Router.dir/build: main/Router

.PHONY : main/CMakeFiles/Router.dir/build

main/CMakeFiles/Router.dir/clean:
	cd /home/randy/ece5960-Physical-Design/PA3/build/main && $(CMAKE_COMMAND) -P CMakeFiles/Router.dir/cmake_clean.cmake
.PHONY : main/CMakeFiles/Router.dir/clean

main/CMakeFiles/Router.dir/depend:
	cd /home/randy/ece5960-Physical-Design/PA3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/randy/ece5960-Physical-Design/PA3 /home/randy/ece5960-Physical-Design/PA3/main /home/randy/ece5960-Physical-Design/PA3/build /home/randy/ece5960-Physical-Design/PA3/build/main /home/randy/ece5960-Physical-Design/PA3/build/main/CMakeFiles/Router.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : main/CMakeFiles/Router.dir/depend

