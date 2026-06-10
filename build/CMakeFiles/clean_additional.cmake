# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\cadalytic_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\cadalytic_autogen.dir\\ParseCache.txt"
  "cadalytic_autogen"
  "geometry\\CMakeFiles\\geometry_autogen.dir\\AutogenUsed.txt"
  "geometry\\CMakeFiles\\geometry_autogen.dir\\ParseCache.txt"
  "geometry\\geometry_autogen"
  )
endif()
