file(REMOVE_RECURSE
  "doc/doxygen-html"
  "doc/doxygen.log"
  "doc/tsid.doxytag"
  "CMakeFiles/NightlyBuild"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/NightlyBuild.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
