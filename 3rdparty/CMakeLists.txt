cmake_minimum_required(VERSION 2.8.12)
project(RW3DM_THIRDPARTY)

# Add OpenNURBS
include(opennurbs.cmake)

# Add pybind11
add_subdirectory(pybind11)
