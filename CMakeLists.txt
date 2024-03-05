# ----------------------------------------------------------------------
# Finite State Machine Compiler
#
# Copyright 2023, Torsten Rupp
# ----------------------------------------------------------------------

# required cmake version
cmake_minimum_required(VERSION 3.5)

# project name
project(fsmc C CXX)

# cmake includes
include(GNUInstallDirs)

# check if out-of-source build
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Out-of-source-build required. Please create a separate build directory and call cmake inside that directory.")
endif ()

# version
set(versionMajor 1)
set(versionMinor 0)
set(versionPatch 0)

# find programs, packages
find_program(FLEX  flex  REQUIRED)
find_program(BYACC byacc REQUIRED)

# additional compiler flags
if (WERROR)
  string(JOIN " " CMAKE_C_FLAGS   ${CMAKE_C_FLAGS}   "-Werror")
  string(JOIN " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Werror")
endif()

# compiler/linker flags
string(JOIN " " CMAKE_C_FLAGS                ${CMAKE_C_FLAGS}           "-Wall")
string(JOIN " " CMAKE_C_FLAGS_DEBUG          ${CMAKE_C_FLAGS_DEBUG}            )
string(JOIN " " CMAKE_C_FLAGS_RELEASE        ${CMAKE_C_FLAGS_RELEASE}          )
string(JOIN " " CMAKE_CXX_FLAGS              ${CMAKE_CXX_FLAGS}         "-Wall")
string(JOIN " " CMAKE_CXX_FLAGS_DEBUG        ${CMAKE_CXX_FLAGS_DEBUG}          )
string(JOIN " " CMAKE_CXX_FLAGS_RELEASE      ${CMAKE_CXX_FLAGS_RELEASE}        )
string(JOIN " " CMAKE_EXE_LINKER_FLAGS       ${CMAKE_EXE_LINKER_FLAGS}         )
string(JOIN " " CMAKE_EXE_LINKER_FLAGS_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG}   )
if (NOT ${CMAKE_CROSSCOMPILING})
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# global definitions
add_compile_definitions(
)

# global include search paths
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
)

# global library search paths
link_directories(
)

# create scanner
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
  COMMAND
    flex -o scanner.cpp ${CMAKE_SOURCE_DIR}/scanner.l
  DEPENDS
    ${CMAKE_SOURCE_DIR}/scanner.l
    ${CMAKE_SOURCE_DIR}/location.h
)

# create parser
add_custom_target(parser.tab.c
  COMMAND
    byacc -B -b parser -d -t ${CMAKE_SOURCE_DIR}/parser.y
  BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
  DEPENDS
    ${CMAKE_SOURCE_DIR}/parser.y
    ${CMAKE_SOURCE_DIR}/location.h
)

# executable
add_executable(fsmc
  fsmc.cpp
  parser.cpp
  ast.cpp
  visitor.cpp
  codegenerator.cpp
  dotgenerator.cpp
  
  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
)
add_dependencies(fsmc
  parser.tab.c
)

# install
install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/fsmc
  DESTINATION bin
  COMPONENT runtime
)

# info
message(STATUS "Project   : ${CMAKE_PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Version   : ${versionMajor}.${versionMinor}.${versionPatch}")
