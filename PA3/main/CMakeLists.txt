add_executable(Router main.cpp)

# options/flags pass to the compiler
target_compile_features(router INTERFACE cxx_std_17)
target_compile_options(router INTERFACE -Wall -Wextra -Wfatal-errors)

# include and libary linking
target_link_libraries(Router LINK_PUBLIC router)
target_include_directories(Router PRIVATE ${PROJECT_SOURCE_DIR})

