cmake_minimum_required(VERSION 3.15)

project(graphics_ditties)

enable_language(CXX)

add_subdirectory(opengl)
add_subdirectory(vulkan)
if(APPLE)
  enable_language(Swift)
  add_subdirectory(metal)
elseif(WIN32)
  add_subdirectory(d3d12)
endif()
