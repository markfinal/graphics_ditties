cmake_minimum_required(VERSION 3.15)

project(graphics_ditties)

if(CMAKE_XCODE_BUILD_SYSTEM VERSION_GREATER_EQUAL 12)
  cmake_policy(SET CMP0114 NEW)
endif()

# as FetchContent is quiet by default
set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "" FORCE)

enable_language(CXX)

add_subdirectory(opengl)
add_subdirectory(vulkan)
if(APPLE)
  enable_language(Swift)
  add_subdirectory(metal)
elseif(WIN32)
  add_subdirectory(d3d11)
  add_subdirectory(d3d12)
endif()
