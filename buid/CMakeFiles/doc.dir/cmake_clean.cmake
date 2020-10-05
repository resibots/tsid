file(REMOVE_RECURSE
  "doc/doxygen-html"
  "doc/doxygen.log"
  "doc/tsid.doxytag"
  "CMakeFiles/doc"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/doc.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
