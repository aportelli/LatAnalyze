set(UTILS_STEM
    plot
    resample
    sample-combine
    sample-dwt
    sample-element
    sample-fake
    sample-ft
    sample-merge
    sample-plot-corr
    sample-plot
    sample-read)

foreach(_stem ${UTILS_STEM})
  add_executable(latan-${_stem} ${_stem}.cpp)
  target_link_libraries(latan-${_stem} LatAnalyze)
  install(TARGETS latan-${_stem} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()
