# CMakeList.txt : CMake project for MO1 Opesy, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (CMakeTarget "MO1 Opesy.cpp" "MO1 Opesy.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
