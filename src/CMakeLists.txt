# CMakeList.txt : CMake project for MO1 Opesy, include source and define
# project specific logic here.
#

# for ascii art with utf, which is apparently msvc-only
if(MSVC)
	add_compile_options(/utf-8)
endif()

# Add source to this project's executable.
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(csopesy ${SOURCES})
target_include_directories(csopesy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET csopesy PROPERTY CXX_STANDARD 20)
endif()

# copy config.txt into the build folder right next to the exe
configure_file(${CMAKE_SOURCE_DIR}/config.txt ${CMAKE_CURRENT_BINARY_DIR}/config.txt COPYONLY)
