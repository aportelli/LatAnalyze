set(EXAMPLES_STEM
    exCompiledDoubleFunction
    exDerivative
    exDWT
    exFit
    exFitSample
    exIntegrator
    exInterp
    exMat
    exMathInterpreter
    exMin
    exPlot
    exPValue
    exRand
    exRootFinder
    exThreadPool)

foreach(_stem ${EXAMPLES_STEM})
  add_executable(${_stem} ${_stem}.cpp)
  target_link_libraries(${_stem} LatAnalyze compiler_flags)
endforeach()
