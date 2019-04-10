# Include jsoncpp source
set(JSONCPP_SRC 
  ${CMAKE_CURRENT_LIST_DIR}/jsoncpp/jsoncpp.cpp
  ${CMAKE_CURRENT_LIST_DIR}/jsoncpp/json/json.h
  ${CMAKE_CURRENT_LIST_DIR}/jsoncpp/json/json-forwards.h
)

# Compile jsoncpp as a static library
add_library(jsoncpp STATIC ${JSONCPP_SRC})
set_target_properties(jsoncpp PROPERTIES DEBUG_POSTFIX "_d")
set_property(TARGET jsoncpp PROPERTY CXX_STANDARD 17)
target_include_directories(jsoncpp PUBLIC "${CMAKE_CURRENT_LIST_DIR}/jsoncpp")
