file(REMOVE_RECURSE
  "doc/doxygen-html"
  "doc/doxygen.log"
  "doc/tsid.doxytag"
  "CMakeFiles/ContinuousCoverage"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ContinuousCoverage.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
