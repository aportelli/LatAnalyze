set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

include(FindBISON)
include(FindFLEX)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(
  AsciiParser LatAnalyze/Io/AsciiParser.ypp
  ${CMAKE_CURRENT_BINARY_DIR}/AsciiParser.cpp COMPILE_FLAGS "-Wno-deprecated")
flex_target(AsciiLexer LatAnalyze/Io/AsciiLexer.lpp
            ${CMAKE_CURRENT_BINARY_DIR}/AsciiLexer.cpp)
add_flex_bison_dependency(AsciiLexer AsciiParser)

bison_target(
  MathParser LatAnalyze/Core/MathParser.ypp
  ${CMAKE_CURRENT_BINARY_DIR}/MathParser.cpp COMPILE_FLAGS "-Wno-deprecated")
flex_target(MathLexer LatAnalyze/Core/MathLexer.lpp
            ${CMAKE_CURRENT_BINARY_DIR}/MathLexer.cpp)
add_flex_bison_dependency(MathLexer MathParser)

add_library(LatAnalyze SHARED)
file(GLOB_RECURSE EIGEN_HEADERS Eigen/*)
target_include_directories(LatAnalyze PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_sources(
  LatAnalyze
  PRIVATE ${BISON_AsciiParser_OUTPUTS}
          ${FLEX_AsciiLexer_OUTPUTS}
          ${BISON_MathParser_OUTPUTS}
          ${FLEX_MathLexer_OUTPUTS}
          ${CMAKE_CURRENT_BINARY_DIR}/config.h
          LatAnalyze/Core/Exceptions.cpp
          LatAnalyze/Core/Mat.cpp
          LatAnalyze/Core/Math.cpp
          LatAnalyze/Core/MathInterpreter.cpp
          LatAnalyze/Core/OptParser.cpp
          LatAnalyze/Core/Plot.cpp
          LatAnalyze/Core/ThreadPool.cpp
          LatAnalyze/Core/Utilities.cpp
          LatAnalyze/Functional/CompiledFunction.cpp
          LatAnalyze/Functional/CompiledModel.cpp
          LatAnalyze/Functional/Function.cpp
          LatAnalyze/Functional/Model.cpp
          LatAnalyze/Functional/TabFunction.cpp
          LatAnalyze/Global.cpp
          LatAnalyze/Io/AsciiFile.cpp
          LatAnalyze/Io/BinReader.cpp
          LatAnalyze/Io/File.cpp
          LatAnalyze/Io/Hdf5File.cpp
          LatAnalyze/Io/Io.cpp
          LatAnalyze/Io/Xml/tinyxml2.cpp
          LatAnalyze/Io/XmlReader.cpp
          LatAnalyze/Numerical/DWT.cpp
          LatAnalyze/Numerical/DWTFilters.cpp
          LatAnalyze/Numerical/Derivative.cpp
          LatAnalyze/Numerical/GslFFT.cpp
          LatAnalyze/Numerical/GslHybridRootFinder.cpp
          LatAnalyze/Numerical/GslMinimizer.cpp
          LatAnalyze/Numerical/GslQagsIntegrator.cpp
          LatAnalyze/Numerical/Minimizer.cpp
          LatAnalyze/Numerical/MinuitMinimizer.cpp
          LatAnalyze/Numerical/NloptMinimizer.cpp
          LatAnalyze/Numerical/RootFinder.cpp
          LatAnalyze/Numerical/Solver.cpp
          LatAnalyze/Physics/CorrelatorFitter.cpp
          LatAnalyze/Physics/DataFilter.cpp
          LatAnalyze/Physics/EffectiveMass.cpp
          LatAnalyze/Statistics/FitInterface.cpp
          LatAnalyze/Statistics/Histogram.cpp
          LatAnalyze/Statistics/Random.cpp
          LatAnalyze/Statistics/StatArray.cpp
          LatAnalyze/Statistics/XYSampleData.cpp
          LatAnalyze/Statistics/XYStatData.cpp
  PUBLIC FILE_SET
         public_headers
         TYPE
         HEADERS
         FILES
         ${EIGEN_HEADERS}
         LatAnalyze/Core/Eigen.hpp
         LatAnalyze/Core/EigenPlugin.hpp
         LatAnalyze/Core/Exceptions.hpp
         LatAnalyze/Core/Mat.hpp
         LatAnalyze/Core/Math.hpp
         LatAnalyze/Core/MathInterpreter.hpp
         LatAnalyze/Core/OptParser.hpp
         LatAnalyze/Core/ParserState.hpp
         LatAnalyze/Core/Plot.hpp
         LatAnalyze/Core/ThreadPool.hpp
         LatAnalyze/Core/Utilities.hpp
         LatAnalyze/Core/stdincludes.hpp
         LatAnalyze/Functional/CompiledFunction.hpp
         LatAnalyze/Functional/CompiledModel.hpp
         LatAnalyze/Functional/Function.hpp
         LatAnalyze/Functional/Model.hpp
         LatAnalyze/Functional/TabFunction.hpp
         LatAnalyze/Global.hpp
         LatAnalyze/Io/AsciiFile.hpp
         LatAnalyze/Io/BinReader.hpp
         LatAnalyze/Io/File.hpp
         LatAnalyze/Io/Hdf5File.hpp
         LatAnalyze/Io/Io.hpp
         LatAnalyze/Io/IoObject.hpp
         LatAnalyze/Io/Xml/tinyxml2.hpp
         LatAnalyze/Io/XmlReader.hpp
         LatAnalyze/Numerical/DWT.hpp
         LatAnalyze/Numerical/DWTFilters.hpp
         LatAnalyze/Numerical/Derivative.hpp
         LatAnalyze/Numerical/FFT.hpp
         LatAnalyze/Numerical/GslFFT.hpp
         LatAnalyze/Numerical/GslHybridRootFinder.hpp
         LatAnalyze/Numerical/GslMinimizer.hpp
         LatAnalyze/Numerical/GslQagsIntegrator.hpp
         LatAnalyze/Numerical/Integrator.hpp
         LatAnalyze/Numerical/Minimizer.hpp
         LatAnalyze/Numerical/MinuitMinimizer.hpp
         LatAnalyze/Numerical/NloptMinimizer.hpp
         LatAnalyze/Numerical/RootFinder.hpp
         LatAnalyze/Numerical/Solver.hpp
         LatAnalyze/Physics/CorrelatorFitter.hpp
         LatAnalyze/Physics/DataFilter.hpp
         LatAnalyze/Physics/EffectiveMass.hpp
         LatAnalyze/Statistics/Dataset.hpp
         LatAnalyze/Statistics/FitInterface.hpp
         LatAnalyze/Statistics/Histogram.hpp
         LatAnalyze/Statistics/MatSample.hpp
         LatAnalyze/Statistics/Random.hpp
         LatAnalyze/Statistics/StatArray.hpp
         LatAnalyze/Statistics/XYSampleData.hpp
         LatAnalyze/Statistics/XYStatData.hpp
         LatAnalyze/includes.hpp)
target_link_libraries(LatAnalyze PUBLIC GSL::gsl hdf5::hdf5 hdf5::hdf5_cpp
                                        Minuit2::Minuit2 NLopt::nlopt)
