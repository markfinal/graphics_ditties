cmake_minimum_required(VERSION 3.15)

project(graphics_ditties)

if(WIN32)
  add_subdirectory(d3d12)
endif()
