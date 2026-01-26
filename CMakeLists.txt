cmake_minimum_required(VERSION 3.22)

# -----------------------
# Project / Language setup
# -----------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_PROJECT_NAME high-voltage-controller)

# Options to select build "mode"
option(EMBEDDED_BUILD "Build firmware for STM32" ON)
option(BUILD_TESTS    "Build unit tests (host)"  OFF)

# Default build type (single-config generators like Ninja/Makefiles)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "EMBEDDED_BUILD: ${EMBEDDED_BUILD}")
message(STATUS "BUILD_TESTS: ${BUILD_TESTS}")

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# If embedded, bring in the ARM toolchain
if(EMBEDDED_BUILD)
  include("cmake/gcc-arm-none-eabi.cmake")
endif()

# Define the project after toolchain selection
project(${CMAKE_PROJECT_NAME} C)

# Enable languages
if(EMBEDDED_BUILD)
  enable_language(ASM)
endif()

# -----------------------
# Firmware target (embedded)
# -----------------------
if(EMBEDDED_BUILD)
  add_executable(${CMAKE_PROJECT_NAME})

  # Add STM32CubeMX generated sources/libraries
  add_subdirectory(cmake/stm32cubemx)

  set(USER_SRC_DIR ${CMAKE_SOURCE_DIR}/Core/User)

  target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SRC_DIR}/app_main.c
    ${USER_SRC_DIR}/hv_control.c
    ${USER_SRC_DIR}/heartbeat.c
    ${USER_SRC_DIR}/adc_driver.c
    ${USER_SRC_DIR}/debug_uart.c
    ${USER_SRC_DIR}/can_driver.c
    ${USER_SRC_DIR}/relay.c
    ${USER_SRC_DIR}/hv_control_logic.c
  )

  target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SRC_DIR}
  )

  target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
  )

  # Tool for flashing
  set(STM32_PROGRAMMER_CLI "STM32_Programmer_CLI" CACHE FILEPATH "Path to STM32_Programmer_CLI")

  if(NOT STM32_PROGRAMMER_CLI)
    message(FATAL_ERROR "STM32_PROGRAMMER_CLI not set. Please define the path using -DSTM32_PROGRAMMER_CLI=...")
  endif()

  add_custom_target(flash
    COMMAND ${STM32_PROGRAMMER_CLI} -c port=SWD -d $<TARGET_FILE:${CMAKE_PROJECT_NAME}> -rst
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Flashing firmware to target via ST-Link..."
  )
endif()

# -----------------------
# Unit tests (host)
# -----------------------
if(BUILD_TESTS)
  if(EMBEDDED_BUILD)
    message(FATAL_ERROR "BUILD_TESTS=ON requires EMBEDDED_BUILD=OFF (tests must be built with the host compiler).")
  endif()
  include(CTest)
  enable_testing()
  add_subdirectory(Core/Test)
endif()
