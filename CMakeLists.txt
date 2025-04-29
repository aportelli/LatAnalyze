# package config
cmake_minimum_required(VERSION 3.24.0)
project(
  LatAnalyze
  VERSION 3.6
  LANGUAGES C CXX)

# includes
include(CheckCXXCompilerFlag)
include(CMakePackageConfigHelpers)
include(FetchContent)
include(FindPackageMessage)
include(GNUInstallDirs)

# C++ compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
  set(MARCH_FLAG "-march=native")
endif()
check_cxx_compiler_flag("-mtune=native" COMPILER_SUPPORTS_MTUNE_NATIVE)
if(COMPILER_SUPPORTS_MTUNE_NATIVE)
  set(MTUNE_FLAG "-mtune=native")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${MARCH_FLAG} ${MTUNE_FLAG}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O3 -g -DNDEBUG ${MARCH_FLAG} ${MTUNE_FLAG}")
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)
target_compile_options(
  compiler_flags
  INTERFACE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>")

# position idependent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# installation rpath setup
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# fetch and create symbolic link to Eigen headers
set(EIGEN_URL "https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz")
set(EIGEN_TGZ "${CMAKE_BINARY_DIR}/eigen-3.4.0.tar.gz")
set(EIGEN_DIR "${CMAKE_BINARY_DIR}/deps/eigen-src")
if(NOT EXISTS "${EIGEN_DIR}/Eigen/Dense")
    file(DOWNLOAD "${EIGEN_URL}" "${EIGEN_TGZ}" SHOW_PROGRESS)
    file(ARCHIVE_EXTRACT INPUT "${EIGEN_TGZ}" DESTINATION "${CMAKE_BINARY_DIR}/deps")
    file(GLOB EIGEN_EXTRACTED_DIR "${CMAKE_BINARY_DIR}/deps/eigen-*")
    file(RENAME "${EIGEN_EXTRACTED_DIR}" "${EIGEN_DIR}")
    message(STATUS "Eigen3 fetched")
endif()
file(CREATE_LINK ${EIGEN_DIR}/Eigen ${CMAKE_SOURCE_DIR}/lib/LatAnalyze/Eigen SYMBOLIC)

# dependencies
find_package(Threads REQUIRED)
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
