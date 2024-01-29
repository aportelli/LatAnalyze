cmake_minimum_required(VERSION 3.11.0)
project(
  LatAnalyze
  VERSION 3.6
  LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
include(FindPackageMessage)

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

find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C CXX)
find_package(Minuit2)
if(Minuit2_FOUND)
  find_package_message(
    Minuit2
    "Found Minuit2: ${Minuit2_DIR} (found version \"${Minuit2_VERSION}\")" " ")
else()
  message(STATUS "Minuit2 not found")
endif()
find_package(NLopt)
if(NLopt_FOUND)
  find_package_message(
    NLopt "Found NLopt: ${NLopt_DIR} (found version \"${NLopt_VERSION}\")" " ")
else()
  message(STATUS "NLopt not found")
endif()

add_subdirectory(lib)
