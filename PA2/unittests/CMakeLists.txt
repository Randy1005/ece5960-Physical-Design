enable_testing()

include(${FP_3RD_PARTY_DIR}/doctest/doctest.cmake)

list(APPEND FP_UNITTESTS
  verify_parse
)

foreach(unittest IN LISTS FP_UNITTESTS)
  add_executable(${unittest} ${unittest}.cpp)
  target_include_directories(${unittest} PRIVATE ${PROJECT_SOURCE_DIR} ${FP_3RD_PARTY_DIR}/doctest)
  target_link_libraries(${unittest} LINK_PUBLIC floorplanner LINK_PUBLIC stdc++fs)
  doctest_discover_tests(${unittest})
endforeach()

