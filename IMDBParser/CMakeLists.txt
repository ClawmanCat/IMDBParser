cmake_minimum_required (VERSION 3.12)

file(GLOB_RECURSE project_sources CONFIGURE_DEPENDS LIST_DIRECTORIES false "*.hpp" "*.cpp" "*.h" "*.c")
add_executable(IMDBParser ${project_sources})