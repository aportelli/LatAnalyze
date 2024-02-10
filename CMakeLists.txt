# package config
cmake_minimum_required(VERSION 3.11.0)
project(
  LatAnalyze
  VERSION 3.6
  LANGUAGES C CXX)

# includes
include(CMakePackageConfigHelpers)
include(FetchContent)
include(FindPackageMessage)
include(GNUInstallDirs)

# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# installation rpath setup
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# fetch and create symbolic link to Eigen headers
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/deps)
FetchContent_Declare(
  Eigen3
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0)
FetchContent_GetProperties(Eigen3)
if(NOT eigen3_POPULATED)
  FetchContent_Populate(Eigen3)
  message(STATUS "Eigen3 fetched")
endif()
file(CREATE_LINK ${eigen3_SOURCE_DIR}/Eigen ${CMAKE_SOURCE_DIR}/lib/LatAnalyze/Eigen
     SYMBOLIC)

# dependencies
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C CXX)
find_package(Minuit2 QUIET)
if(Minuit2_FOUND)
  set(Minuit2_MSG "yes")
  find_package_message(
    Minuit2
    "Found Minuit2: ${Minuit2_DIR} (found version \"${Minuit2_VERSION}\")" " ")
else()
  set(Minuit2_MSG "no")
  message(STATUS "Minuit2 not found")
endif()
find_package(NLopt QUIET)
if(NLopt_FOUND)
  set(NLopt_MSG "yes")
  find_package_message(
    NLopt "Found NLopt: ${NLopt_DIR} (found version \"${NLopt_VERSION}\")" " ")
else()
  set(NLopt_MSG "no")
  message(STATUS "NLopt not found")
endif()

# subdirectories
add_subdirectory(lib)
add_subdirectory(examples)
add_subdirectory(physics)
add_subdirectory(utils)

# summary
message(STATUS "---------------------------")
message(STATUS "LatAnalyze v${PROJECT_VERSION} configured")
message(STATUS "---------------------------")
message(STATUS "    install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Minuit2 minimisers: ${Minuit2_MSG}")
message(STATUS "  NLopt minimisers: ${NLopt_MSG}")
message(STATUS "---------------------------")
