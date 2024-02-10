set(PHYSICS_STEM 2pt-fit)

foreach(_stem ${PHYSICS_STEM})
  add_executable(latan-${_stem} ${_stem}.cpp)
  target_link_libraries(latan-${_stem} LatAnalyze)
  install(TARGETS latan-${_stem} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()
