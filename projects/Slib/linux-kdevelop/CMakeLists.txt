cmake_minimum_required(VERSION 2.8)
project(slib)

SET(CMAKE_CXX_FLAGS "-std=c++11")

file (GLOB SLIB_CORE_FILES ${CMAKE_CURRENT_LIST_DIR}/../../../inc/slib/core/*.h ${CMAKE_CURRENT_LIST_DIR}/../../../src/slib/core/*.cpp ${CMAKE_CURRENT_LIST_DIR}/../../../inc/slib/math/*.h ${CMAKE_CURRENT_LIST_DIR}/../../../src/slib/math/*.cpp ${CMAKE_CURRENT_LIST_DIR}/../../../inc/slib/crypto/*.h ${CMAKE_CURRENT_LIST_DIR}/../../../src/slib/crypto/*.cpp)
add_library(slib-core ${SLIB_CORE_FILES})

file (GLOB SLIB_NETWORK_FILES ${CMAKE_CURRENT_LIST_DIR}/../../../inc/slib/network/*.h ${CMAKE_CURRENT_LIST_DIR}/../../../src/slib/network/*.cpp)
add_library(slib-network ${SLIB_NETWORK_FILES})

file (GLOB SLIB_DB_FILES ${CMAKE_CURRENT_LIST_DIR}/../../../inc/slib/db/*.h ${CMAKE_CURRENT_LIST_DIR}/../../../src/slib/db/*.cpp)
add_library(slib-db ${SLIB_DB_FILES})

file (GLOB SLIB_WEB_FILES ${CMAKE_CURRENT_LIST_DIR}/../../../inc/slib/web/*.h ${CMAKE_CURRENT_LIST_DIR}/../../../src/slib/web/*.cpp)
add_library(slib-web ${SLIB_WEB_FILES})

add_library(slib-zlib ${CMAKE_CURRENT_LIST_DIR}/../../../src/thirdparty/thirdparty_zlib.c)

target_link_libraries(slib-web slib-db slib-network slib-core pthread slib-zlib)
