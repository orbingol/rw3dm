cmake_minimum_required(VERSION 2.8)
project(RW3DM)

# Required for GenerateExportHeader module
include(GenerateExportHeader)

# Generate a Visual Studio filter "CMakePredefinedTargets"
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
endif()

# Set install directory
set(RW3DM_INSTALL_DIR ${PROJECT_BINARY_DIR}/install CACHE PATH "Installation directory")
set(CMAKE_INSTALL_PREFIX ${RW3DM_INSTALL_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)

# Add 3rd party libraries
add_subdirectory(3rdparty/)
include_directories(3rdparty/opennurbs)

# 3DM Reading and Writing Library
set(RW3DM_LIB_SRC
  ${PROJECT_BINARY_DIR}/rw3dm_export.h
  src/rw3dm.h
  src/rw3dm.cpp
)
add_library(rw3dm SHARED ${RW3DM_LIB_SRC})
generate_export_header(rw3dm)
set_target_properties(rw3dm PROPERTIES DEBUG_POSTFIX "_d")
target_compile_definitions(rw3dm PRIVATE -DOPENNURBS_IMPORTS)  # OpenNURBS dynamic linking
target_include_directories(rw3dm PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(rw3dm PRIVATE opennurbs)
install(TARGETS rw3dm DESTINATION ${RW3DM_INSTALL_DIR})

# Testing application
set(RW3DM_APP_SRC
  app/main.cpp
)
add_executable(rw3dmapp ${RW3DM_APP_SRC})
set_target_properties(rw3dmapp PROPERTIES DEBUG_POSTFIX "_d")
target_link_libraries(rw3dmapp PRIVATE rw3dm)
install(TARGETS rw3dmapp DESTINATION ${RW3DM_INSTALL_DIR})
