CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(MongoInterface)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(USE_SUPERBUILD "Use superbuild to pull in and build dependencies." ON)
set(DOXYGEN_DIR CACHE STRING "${CMAKE_BINARY_DIR}/Doxygen")

# Superbuild rules
if(USE_SUPERBUILD)
    option(GIT_HTTPS "Clone git repositories with HTTPS" ON)
    include(ExternalProject)
    include(Superbuild.cmake)
    return()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

list(APPEND CMAKE_MODULE_PATH ${MongoInterface_SOURCE_DIR} ${MongoInterface_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

FIND_PACKAGE(Boost 1.54 COMPONENTS system regex thread filesystem program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
FIND_PACKAGE(Threads REQUIRED)
#INCLUDE_DIRECTORIES( ${Threads_INCLUDE_DIR})
FIND_PACKAGE(JsonBox REQUIRED)
INCLUDE_DIRECTORIES( ${JSONBOX_INCLUDE_DIRS})

#FIND_PACKAGE(MongoDB REQUIRED)
find_package(BSON-1.0 REQUIRED)
find_package(BSONCxx REQUIRED)
find_package(MongoC REQUIRED)
find_package(MongoCxx REQUIRED)


message("BSON: ${BSON-1.0_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES( ${MONGOC_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIRS} ${BSON-1.0_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS})


MESSAGE(STATUS "LIBRARY STATUS:")
MESSAGE(STATUS "Boost: " ${Boost_INCLUDE_DIR})
#MESSAGE(STATUS "Threads: " ${Threads_INCLUDE_DIR})
MESSAGE(STATUS "JsonBox: " ${JSONBOX_INCLUDE_DIRS})
#MESSAGE(STATUS "MongoDB: " ${MongoDB_INCLUDE_DIR})

message("MongoCxx: ${MONGOCXX_LIBRARY}")


add_library(MongoInterface STATIC MongoInterface.cpp MongoInterface.h)
TARGET_LINK_LIBRARIES(MongoInterface ${JSONBOX_LIBRARY_RELEASE} ${MONGOC_LIBRARY} ${MONGOCXX_LIBRARY} ${BSONCXX_LIBRARY} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS MongoInterface ARCHIVE DESTINATION lib)
install(FILES MongoInterface.h DESTINATION include)

add_executable(MongoInterfaceTest mongotest/MongoInterface_test.cpp MongoInterface.h)
TARGET_LINK_LIBRARIES(MongoInterfaceTest MongoInterface ${JSONBOX_LIBRARY_RELEASE} ${MONGOC_LIBRARY} ${MONGOCXX_LIBRARY} ${BSONCXX_LIBARY} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS MongoInterfaceTest DESTINATION test)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
file(MAKE_DIRECTORY ${DOXYGEN_DIR})
configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${DOXYGEN_DIR}/Doxyfile @ONLY)
add_custom_target(doc ALL
${DOXYGEN_EXECUTABLE} ${DOXYGEN_DIR}/Doxyfile
WORKING_DIRECTORY ${DOXYGEN_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

MESSAGE(STATUS "DOXYGEN_DIR: " ${DOXYGEN_DIR})
