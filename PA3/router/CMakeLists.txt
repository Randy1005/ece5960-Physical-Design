add_library(router router.cpp)

set_property(TARGET router PROPERTY CXX_STANDARD 17)
target_compile_options(router INTERFACE -Wall -Wextra -Wfatal-errors)
