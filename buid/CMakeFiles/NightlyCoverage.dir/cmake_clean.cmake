file(REMOVE_RECURSE
  "doc/doxygen-html"
  "doc/doxygen.log"
  "doc/tsid.doxytag"
  "CMakeFiles/NightlyCoverage"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/NightlyCoverage.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
