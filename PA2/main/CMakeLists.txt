add_executable(fp main.cpp)

# options/flags pass to the compiler
target_compile_features(floorplanner INTERFACE cxx_std_17)
target_compile_options(floorplanner INTERFACE -Wall -Wextra -Wfatal-errors)

# include and libary linking
target_link_libraries(fp LINK_PUBLIC floorplanner)
target_include_directories(fp PRIVATE ${PROJECT_SOURCE_DIR})

