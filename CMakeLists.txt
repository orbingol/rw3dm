cmake_minimum_required(VERSION 2.8.12)
project(RW3DM)

# Generate a Visual Studio filter "CMakePredefinedTargets"
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
endif()

# Set install prefix
set(RW3DM_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install CACHE PATH "Install prefix")

# Set python module name
set(RW3DM_MODULE_NAME "rw3dm" CACHE STRING "Python module name")

# Set python module install directory
set(RW3DM_INSTALL_DIR ${RW3DM_INSTALL_PREFIX}/${RW3DM_MODULE_NAME} CACHE INTERNAL "Python module installation directory")
set(CMAKE_INSTALL_PREFIX ${RW3DM_INSTALL_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)

# Add python libraries
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# Add 3rd party libraries
add_subdirectory(3rdparty/)

#
# OpenNURBS/Rhino .3DM file reading and writing library
#

# Set source files
set(RW3DM_MODULE_SRC
  src/rw3dm.h
  src/rw3dm.cpp
  src/bindings.cpp
)

# Generate python module with pybind11
pybind11_add_module(rw3dm MODULE ${RW3DM_MODULE_SRC})
target_include_directories(rw3dm PRIVATE ${PROJECT_BINARY_DIR})
target_compile_definitions(rw3dm
  PRIVATE -DOPENNURBS_IMPORTS   # OpenNURBS dynamic linking
)
target_link_libraries(rw3dm PRIVATE opennurbs)
set_target_properties(rw3dm PROPERTIES DEBUG_POSTFIX "_d")
install(TARGETS rw3dm DESTINATION ${RW3DM_INSTALL_DIR})

#
# Generate __init__.py file
#

# Custom target to generate __init__py.file
add_custom_target(generate_init_py
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
)

# Add generate_init_py target as a dependency to the main python module
add_dependencies(rw3dm generate_init_py)

# Install __init__.py to the module/app directory
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
  DESTINATION ${RW3DM_INSTALL_DIR}
)

#
# Uninstall target
# Ref: https://gitlab.kitware.com/cmake/community/wikis/FAQ#can-i-do-make-uninstall-with-cmake
#

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
  )
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  )
endif()
