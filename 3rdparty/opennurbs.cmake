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
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.skip")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.cpp" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.skip")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.cpp" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.skip")
file(GLOB ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.cpp")

# zlib source (included with OpenNURBS)
file(GLOB ON_ZLIB_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.c")

# Generate zlib as a static library
add_library(zlib STATIC ${ON_ZLIB_SRC})

# Add zlib to the link libraries
set(ON_LINK_LIBRARIES zlib)

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
  file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.c" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.skip")
  file(GLOB ON_UUID_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/*.c")
  file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.c")

  # Generate uuid as a static library
  add_library(uuid STATIC ${ON_UUID_SRC})

  # Add uuid to the link libraries
  set(ON_LINK_LIBRARIES ${ON_LINK_LIBRARIES} uuid)

  # Update compiler directives
  set(ON_COMP_DIRECTIVES
    ${ON_COMP_DIRECTIVES}
    -DON_RUNTIME_LINUX
    -DON_CLANG_CONSTRUCTOR_BUG
  )
endif()

# Generate OpenNURBS library
if(RW3DM_BUILD_ON_DLL)
  # Generate a shared library
  add_library(opennurbs SHARED ${ON_SRC})

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
  add_library(opennurbs STATIC ${ON_SRC})
endif()

# Link OpenNURBS library
target_link_libraries(opennurbs PRIVATE ${ON_LINK_LIBRARIES})

# Add compiler definitions for OpenNURBS
target_compile_definitions(opennurbs
  PRIVATE ${ON_COMP_DIRECTIVES}
)

# Add OpenNURBS source directory to the global includes
target_include_directories(opennurbs PUBLIC "${CMAKE_CURRENT_LIST_DIR}/opennurbs")
