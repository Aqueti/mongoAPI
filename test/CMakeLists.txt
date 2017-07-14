CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MongoTest)

list(APPEND CMAKE_MODULE_PATH ${MongoTest_SOURCE_DIR} ${MongoTest_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

FIND_PACKAGE(MongoInterface REQUIRED)

INCLUDE_DIRECTORIES(${MongoInterface_INCLUDE_DIR})

add_executable(MongoTest MongoInterface_test.cpp)
TARGET_LINK_LIBRARIES(MongoTest
    MongoInterface
)

