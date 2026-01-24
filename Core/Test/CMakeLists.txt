cmake_minimum_required(VERSION 3.22)
project(hvc_tests C)
include(CTest)
enable_testing()

add_executable(hvc_tests
  unity/unity.c
  mock_ts.h
  test_runner.c
  test_hv_control_logic.c
  test_soc.c
  ../User/hv_control_logic.c
)

target_include_directories(hvc_tests PRIVATE
  .
  unity
  ../User
)

target_compile_definitions(hvc_tests PRIVATE UNIT_TEST)

add_test(NAME hvc_tests COMMAND hvc_tests)
