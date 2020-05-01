# Inspired from https://github.com/mcneel/rhino3dm

# Compiler directives
set(ON_COMP_DIRECTIVES
  -D_GNU_SOURCE
  -DON_COMPILING_OPENNURBS
  -DOPENNURBS_INPUT_LIBS_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
  -DOPENNURBS_ZLIB_LIB_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
  -DUNICODE
  # zlib compiler directives
  -DMY_ZCALLOC
  -DZ_PREFIX
)

# OpenNURBS source
file(GLOB ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.cpp")
list(REMOVE_ITEM ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp")
list(REMOVE_ITEM ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.cpp")
list(REMOVE_ITEM ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.cpp")
set(ON_SRC_ALL ${ON_SRC})

# zlib source (included with OpenNURBS)
file(GLOB ON_ZLIB_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.c")

# Generate zlib as a static library
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_library(zlib STATIC ${ON_ZLIB_SRC})
  target_compile_definitions(zlib
    PRIVATE -DMY_ZCALLOC -DZ_PREFIX
  )
  # Add zlib to the link libraries
  set(ON_LINK_LIBRARIES zlib)
endif()

# OpenNURBS fixes for Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Unfortunately, OpenNURBS does not like "WIN32" preprocessor definitions on x64
  if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
    string(REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(ON_COMP_DIRECTIVES ${ON_COMP_DIRECTIVES} -DWIN64)
  endif()

  # Add shlwapi to the link libraries
  set(ON_LINK_LIBRARIES ${ON_LINK_LIBRARIES} shlwapi)
endif()

# OpenNURBS fixes for Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Include UUID source (included with OpenNURBS)
  file(GLOB ON_UUID_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/*.c")
  list(REMOVE_ITEM ON_UUID_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.c")

  # Update compiler directives
  set(ON_COMP_DIRECTIVES
    ${ON_COMP_DIRECTIVES}
    -DON_RUNTIME_LINUX
    -DON_CLANG_CONSTRUCTOR_BUG
  )
endif()

# Generate OpenNURBS library
if(RW3DM_BUILD_ON_DLL)
  # Cannot compile OpenNURBS as a shared library on Linux
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "OpenNURBS shared library generation is not supported on Linux")
  endif()

  # Generate a shared library
  add_library(opennurbs SHARED ${ON_SRC})
  set_target_properties(opennurbs PROPERTIES DEBUG_POSTFIX "d")

  # Update compiler directives
  set(ON_COMP_DIRECTIVES
    ${ON_COMP_DIRECTIVES}
    -DOPENNURBS_EXPORTS
  )

  # Install OpenNURBS DLL
  install(
    TARGETS opennurbs
    DESTINATION ${RW3DM_INSTALL_DIR}
  )
else()
  # Generate a static library
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library(opennurbs STATIC ${ON_SRC})
  else()
    # Need to combine all source files for static linking on Linux
    add_library(opennurbs STATIC ${ON_UUID_SRC} ${ON_ZLIB_SRC} ${ON_SRC})
  endif()
endif()

# Link OpenNURBS library (only Windows)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(opennurbs PRIVATE ${ON_LINK_LIBRARIES})
endif()

# Add compiler definitions for OpenNURBS
target_compile_definitions(opennurbs
  PRIVATE ${ON_COMP_DIRECTIVES}
)

# Add OpenNURBS source directory to the global includes
target_include_directories(opennurbs PUBLIC "${CMAKE_CURRENT_LIST_DIR}/opennurbs")
