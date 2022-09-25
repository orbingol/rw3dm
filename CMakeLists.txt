cmake_minimum_required(VERSION 3.8.2)
project(rw3dm VERSION 2.1.4)

# Generate a Visual Studio filter "CMakePredefinedTargets"
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Set install directory
set(RW3DM_INSTALL_DIR ${PROJECT_BINARY_DIR}/install CACHE PATH "Install directory")
set(CMAKE_INSTALL_PREFIX ${RW3DM_INSTALL_DIR})

# Options
set(RW3DM_BUILD_ON2JSON ON CACHE BOOL "Compile and install OpenNURBS to JSON converter")
set(RW3DM_BUILD_JSON2ON ON CACHE BOOL "Compile and install JSON to OpenNURBS converter")
set(RW3DM_BUILD_CCCON ON CACHE BOOL "Compile and install  CCC")
set(RW3DM_BUILD_ON_DLL








































































































        OFF CACHE BOOL "Dynamically link OpenNURBS library")

# Set common runtime output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Configure version header file
configure_file(rw3dmConfig.h.in rw3dmConfig.h)

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

# Generate rw3dm core as a static library
set(SOURCE_FILES_RW3DMLIB
  src/rw3dm/common.h
  src/rw3dm/common.cpp
  src/rw3dm/rw3dm.h
  src/rw3dm/rw3dm.cpp
        )
add_library(rw3dm STATIC ${SOURCE_FILES_RW3DMLIB})
target_link_libraries(rw3dm PRIVATE jsoncpp opennurbs)
target_include_directories(rw3dm
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/src/rw3dm"
        "${PROJECT_BINARY_DIR}"
)

if(RW3DM_BUILD_ON2JSON)
  # Set source files for ON2JSON
  set(SOURCE_FILES_ON2JSON
    src/on2json/on2json.h
    src/on2json/on2json.cpp
    src/on2json/main.cpp
  )

  # Generate ON2JSON executable
  add_executable(on2json ${SOURCE_FILES_ON2JSON})
  target_compile_definitions(on2json
      PRIVATE ${BUILD_COMP_DEFS}
  )
  target_link_libraries(on2json PRIVATE jsoncpp opennurbs rw3dm)
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
    src/json2on/json2on.h
    src/json2on/json2on.cpp
    src/json2on/main.cpp
  )

  # Generate executable for JSON2ON
  add_executable(json2on ${SOURCE_FILES_JSON2ON})
  target_compile_definitions(on2json
      PRIVATE ${BUILD_COMP_DEFS}
  )
  target_link_libraries(json2on PRIVATE jsoncpp opennurbs rw3dm)
  set_target_properties(json2on PROPERTIES DEBUG_POSTFIX "d")

  # Install the JSON2ON binary
  install(
    TARGETS json2on
    DESTINATION ${RW3DM_INSTALL_DIR}
  )
endif()

if(RW3DM_BUILD_CCCON)
  # Set source files for JSON2ON
  set(SOURCE_FILES_CCC
          src/ccc/ccc.h
          src/ccc/ccc.cpp
          src/ccc/main.cpp
          )

  # Generate executable for JSON2ON
  add_executable(ccc ${SOURCE_FILES_CCC})
  target_compile_definitions(ccc
          PRIVATE ${BUILD_COMP_DEFS}
          )
  target_link_libraries(ccc PRIVATE jsoncpp opennurbs rw3dm)


  # Install the JSON2ON binary
  install(
          TARGETS ccc
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
