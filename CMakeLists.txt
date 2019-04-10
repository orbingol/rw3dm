cmake_minimum_required(VERSION 3.8.2)
project(RW3DM)

# Generate a Visual Studio filter "CMakePredefinedTargets"
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
endif()

# Set install prefix
set(RW3DM_INSTALL_DIR ${PROJECT_BINARY_DIR}/install CACHE PATH "Install prefix")
set(CMAKE_INSTALL_PREFIX ${RW3DM_INSTALL_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add 3rd party libraries
add_subdirectory(3rdparty)

# Set source files
set(SOURCE_FILES_RW3DM
  src/rw3dm.h
  src/rw3dm.cpp
  src/main.cpp
  src/common.h
  src/common.cpp
)

# Generate executable
add_executable(rw3dm ${SOURCE_FILES_RW3DM})
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_definitions(rw3dm
    PRIVATE -DON_CLANG_CONSTRUCTOR_BUG
  )
endif()
target_link_libraries(rw3dm PRIVATE jsoncpp opennurbs)
set_target_properties(rw3dm PROPERTIES DEBUG_POSTFIX "_d")

# Install the binary
install(
  TARGETS rw3dm 
  DESTINATION ${RW3DM_INSTALL_DIR}
)

# Create uninstall target
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
