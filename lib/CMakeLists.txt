# rpath config
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")

# config.h header for package version and name
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

# find bison & flex
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# ASCII parser
bison_target(
  AsciiParser LatAnalyze/Io/AsciiParser.ypp
  ${CMAKE_CURRENT_BINARY_DIR}/AsciiParser.cpp COMPILE_FLAGS "-Wno-deprecated")
flex_target(AsciiLexer LatAnalyze/Io/AsciiLexer.lpp
            ${CMAKE_CURRENT_BINARY_DIR}/AsciiLexer.cpp)
add_flex_bison_dependency(AsciiLexer AsciiParser)

# math parser
bison_target(
  MathParser LatAnalyze/Core/MathParser.ypp
  ${CMAKE_CURRENT_BINARY_DIR}/MathParser.cpp COMPILE_FLAGS "-Wno-deprecated")
flex_target(MathLexer LatAnalyze/Core/MathLexer.lpp
            ${CMAKE_CURRENT_BINARY_DIR}/MathLexer.cpp)
add_flex_bison_dependency(MathLexer MathParser)

# library target
add_library(LatAnalyze SHARED)
file(GLOB_RECURSE EIGEN_HEADERS LatAnalyze/Eigen/*)
target_include_directories(LatAnalyze PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
include(source-list.cmake)
target_sources(
  LatAnalyze
  PRIVATE ${BISON_AsciiParser_OUTPUTS} ${FLEX_AsciiLexer_OUTPUTS}
          ${BISON_MathParser_OUTPUTS} ${FLEX_MathLexer_OUTPUTS}
          ${CMAKE_CURRENT_BINARY_DIR}/config.h ${LATAN_SOURCES}
  PUBLIC FILE_SET
         public_headers
         TYPE
         HEADERS
         FILES
         ${EIGEN_HEADERS}
         ${LATAN_HEADERS})
target_link_libraries(LatAnalyze PUBLIC GSL::gsl hdf5::hdf5 hdf5::hdf5_cpp)
if(Minuit2_FOUND)
  target_link_libraries(LatAnalyze PUBLIC Minuit2::Minuit2)
endif()
if(NLopt_FOUND)
  target_link_libraries(LatAnalyze PUBLIC NLopt::nlopt)
endif()
set_target_properties(LatAnalyze PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(
  LatAnalyze PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# installation
install(
  TARGETS LatAnalyze
  EXPORT LatAnalyzeTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          FILE_SET public_headers
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
  EXPORT LatAnalyzeTargets
  FILE LatAnalyzeTargets.cmake
  NAMESPACE LatAnalyze::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LatAnalyze)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/LatAnalyzeConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LatAnalyze)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/LatAnalyzeConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/LatAnalyzeConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/LatAnalyzeConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LatAnalyze)
