# CMakeList.txt : CMake project for MO1 Opesy, include source and define
# project specific logic here.
#

# for ascii art with utf
add_compile_options(/utf-8)

# Add source to this project's executable.
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(csopesy ${SOURCES})
target_include_directories(csopesy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET csopesy PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
