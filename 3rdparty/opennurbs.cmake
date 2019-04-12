# Inspired from https://github.com/mcneel/rhino3dm

# OpenNURBS required compiler directives
set(ON_COMP_DIRECTIVES
  -D_GNU_SOURCE
  -DON_COMPILING_OPENNURBS
  -DOPENNURBS_INPUT_LIBS_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
  -DOPENNURBS_ZLIB_LIB_DIR="${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
  -DUNICODE
)

# Unfortunately, OpenNURBS does not like "WIN32" preprocessor definitions on x64
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
    string(REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(ON_COMP_DIRECTIVES ${ON_COMP_DIRECTIVES} -DWIN64)
  endif()
endif()

# Additional compiler directives for Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(ON_COMP_DIRECTIVES
    ${ON_COMP_DIRECTIVES}
    -DON_RUNTIME_LINUX
    -DON_CLANG_CONSTRUCTOR_BUG
  )
endif()

# Include zlib source
file(GLOB ON_ZLIB_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/zlib/*.c")

# Compile zlib as a static library
add_library(zlib STATIC ${ON_ZLIB_SRC})
target_compile_definitions(zlib
  PRIVATE -DMY_ZCALLOC
  PRIVATE -DZ_PREFIX
)
set_property(TARGET zlib PROPERTY CXX_STANDARD 17)

# Set link libraries for OpenNURBS
set(ON_LINK_LIBS zlib)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(ON_LINK_LIBS ${ON_LINK_LIBS} shlwapi)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Include UUID source
  file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.c" "opennurbs/android_uuid/gen_uuid_nt.skip")
  file(GLOB ON_UUID_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/*.c")
  file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/android_uuid/gen_uuid_nt.c")

  # Compile UUID as a static library
  add_library(uuid STATIC ${ON_UUID_SRC})
  set_property(TARGET uuid PROPERTY CXX_STANDARD 17)
  set(ON_LINK_LIBS ${ON_LINK_LIBS} uuid)
endif()

# Include OpenNURBS source
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp" "opennurbs/opennurbs_gl.skip")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.cpp" "opennurbs/opennurbs_unicode_cp932.skip")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.cpp" "opennurbs/opennurbs_unicode_cp949.skip")
file(GLOB ON_SRC "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.h" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/*.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_gl.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp932.cpp")
file(RENAME "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.skip" "${CMAKE_CURRENT_LIST_DIR}/opennurbs/opennurbs_unicode_cp949.cpp")

# Compile OpenNURBS as a static library
add_library(opennurbs STATIC ${ON_SRC})
target_compile_definitions(opennurbs
  PRIVATE ${ON_COMP_DIRECTIVES}
)
target_link_libraries(opennurbs PRIVATE ${ON_LINK_LIBS})
target_include_directories(opennurbs PUBLIC "${CMAKE_CURRENT_LIST_DIR}/opennurbs")
set_property(TARGET opennurbs PROPERTY CXX_STANDARD 17)
