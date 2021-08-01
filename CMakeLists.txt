cmake_minimum_required(VERSION 3.15)

project(graphics_ditties LANGUAGES CXX Swift)

if(APPLE)
  add_subdirectory(metal)
elseif(WIN32)
  add_subdirectory(d3d12)
endif()
