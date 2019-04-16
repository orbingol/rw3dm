cmake_minimum_required(VERSION 3.8.2)
project(rw3dm)

# Generate a Visual Studio filter "CMakePredefinedTargets"
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
endif()

# Set install directory
set(RW3DM_INSTALL_DIR ${PROJECT_BINARY_DIR}/install CACHE PATH "Install directory")
set(CMAKE_INSTALL_PREFIX ${RW3DM_INSTALL_DIR})

# Options
set(RW3DM_BUILD_ON2JSON ON CACHE BOOL "Compile and install OpenNURBS to JSON converter")
set(RW3DM_BUILD_JSON2ON ON CACHE BOOL "Compile and install JSON to OpenNURBS converter")
set(RW3DM_BUILD_ON_DLL OFF CACHE BOOL "Dynamically link OpenNURBS library")

# Set common runtime output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add 3rd party libraries
add_subdirectory(3rdparty)

# Add compiler definitions
if(RW3DM_BUILD_ON_DLL)
  set(BUILD_COMP_DEFS
    ${BUILD_COMP_DEFS}
    -DOPENNURBS_IMPORTS  # OpenNURBS dynamic linking
  )
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(BUILD_COMP_DEFS
    ${BUILD_COMP_DEFS}
    -DON_CLANG_CONSTRUCTOR_BUG  # OpenNURBS fix for Linux compilation
  )
endif()

if(RW3DM_BUILD_ON2JSON)
  # Set source files for ON2JSON
  set(SOURCE_FILES_ON2JSON
    src/rw3dm.h
    src/rw3dm.cpp
    src/common.h
    src/common.cpp
    src/on2json.cpp
  )

  # Generate ON2JSON executable
  add_executable(on2json ${SOURCE_FILES_ON2JSON})
  target_compile_definitions(on2json
      PRIVATE ${BUILD_COMP_DEFS}
  )
  target_link_libraries(on2json PRIVATE jsoncpp opennurbs)
  set_target_properties(on2json PROPERTIES DEBUG_POSTFIX "d")

  # Install the ON2JSON binary
  install(
    TARGETS on2json
    DESTINATION ${RW3DM_INSTALL_DIR}
  )
endif()

if(RW3DM_BUILD_JSON2ON)
  # Set source files for JSON2ON
  set(SOURCE_FILES_JSON2ON
    src/rw3dm.h
    src/rw3dm.cpp
    src/common.h
    src/common.cpp
    src/json2on.cpp
  )

  # Generate executable for JSON2ON
  add_executable(json2on ${SOURCE_FILES_JSON2ON})
  target_compile_definitions(on2json
      PRIVATE ${BUILD_COMP_DEFS}
  )
  target_link_libraries(json2on PRIVATE jsoncpp opennurbs)
  set_target_properties(json2on PROPERTIES DEBUG_POSTFIX "d")

  # Install the JSON2ON binary
  install(
    TARGETS json2on
    DESTINATION ${RW3DM_INSTALL_DIR}
  )
endif()

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
