enable_testing()

include(${ROUTER_3RD_PARTY_DIR}/doctest/doctest.cmake)

list(APPEND ROUTER_UNITTESTS
  example_test
)

foreach(unittest IN LISTS ROUTER_UNITTESTS)
  add_executable(${unittest} ${unittest}.cpp)
  target_include_directories(${unittest} PRIVATE ${PROJECT_SOURCE_DIR} ${ROUTER_3RD_PARTY_DIR}/doctest)
  target_link_libraries(${unittest} LINK_PUBLIC router LINK_PUBLIC stdc++fs)
  doctest_discover_tests(${unittest})
endforeach()

