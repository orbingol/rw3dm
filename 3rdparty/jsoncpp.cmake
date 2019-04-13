# Include jsoncpp source
set(JSONCPP_SRC 
  ${CMAKE_CURRENT_LIST_DIR}/jsoncpp/jsoncpp.cpp
  ${CMAKE_CURRENT_LIST_DIR}/jsoncpp/json/json.h
  ${CMAKE_CURRENT_LIST_DIR}/jsoncpp/json/json-forwards.h
)

# Compile jsoncpp as a static library
add_library(jsoncpp STATIC ${JSONCPP_SRC})
target_include_directories(jsoncpp PUBLIC "${CMAKE_CURRENT_LIST_DIR}/jsoncpp")
