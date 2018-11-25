# Unfortunately, OpenNURBS does not like "WIN32" preprocessor definitions on x64
set(ON_PLATFORM_STR "")
if(MSVC)
  if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
    string(REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(ON_PLATFORM_STR "-DWIN64")
  endif()
endif()

# Include OpenNURBS ZLib source
file(GLOB ON_ZLIB_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.c")

# Include OpenNURBS source
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp" "opennurbs/opennurbs_gl.skip")
file(GLOB ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp")

# Generate ZLib as a static library
add_library(zlib STATIC ${ON_ZLIB_SRC})

# Generate OpenNURBS as a dynamic library
add_library(opennurbs SHARED ${ON_SRC})
target_compile_definitions(opennurbs
  PRIVATE ${ON_PLATFORM_STR}
  PRIBATE -DON_COMPILING_OPENNURBS
  PRIVATE -DOPENNURBS_EXPORTS
  PRIVATE -DOPENNURBS_INPUT_LIBS_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
  PRIVATE -DOPENNURBS_ZLIB_LIB_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
  PRIVATE -DUNICODE
)
set_target_properties(opennurbs PROPERTIES DEBUG_POSTFIX "_d")
target_link_libraries(opennurbs PRIVATE shlwapi zlib)
target_include_directories(opennurbs PUBLIC "${CMAKE_CURRENT_LIST_DIR}/opennurbs")
set_property(TARGET opennurbs PROPERTY CXX_STANDARD 14)

# Install OpenNURBS
install(
  TARGETS opennurbs
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
)
