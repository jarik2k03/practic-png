# TEEEEST
add_executable(
  test-component-bin
  component.test.cxx
)
add_test(NAME TEST_COMPONENTS COMMAND test-component-bin )
target_link_libraries(
  test-component-bin
  PRIVATE GTest::gtest_main
)

target_sources (
  test-component-bin
  PRIVATE FILE_SET my_png 
  TYPE CXX_MODULES
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES component.cppm
)
target_link_libraries (
  test-component-bin
  PRIVATE 
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(test-component-bin)
