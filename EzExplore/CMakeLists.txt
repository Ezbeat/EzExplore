add_library(EzExplore_object OBJECT "ExploreErrors.h" "ExploreFile.cpp" "ExploreFile.h")
add_executable(EzExplore_test "main.cpp" "ExploreErrors.h" "ExploreFile.cpp" "ExploreFile.h")
