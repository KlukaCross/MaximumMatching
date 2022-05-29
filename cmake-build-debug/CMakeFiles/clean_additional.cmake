# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/MaximumMatching_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/MaximumMatching_autogen.dir/ParseCache.txt"
  "MaximumMatching_autogen"
  )
endif()
