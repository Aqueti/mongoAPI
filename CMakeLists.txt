CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)

PROJECT(mongoapi VERSION "0.1.0.0")

option(USE_SUPERBUILD "Use superbuild to pull in and build dependencies." ON)
option(MAKE_TESTS "Build test libraries and excutables." ON)

set(DOXYGEN_DIR CACHE STRING "${CMAKE_BINARY_DIR}/Doxygen")

#Hack? but We can't find bson.h
include_directories(
    ${CMAKE_INSTALL_PREFIX}/include/bsoncxx/v_noabi
    ${CMAKE_INSTALL_PREFIX}/include/libbson-1.0
)

list(APPEND CMAKE_PREFIX_PATH
    ${CMAKE_INSTALL_PREFIX}/lib/cmake
    usr/lib/cmake
)


# Superbuild rules
if(USE_SUPERBUILD)
    include(ExternalProject)
    include(Superbuild.cmake)
    return()
endif()

#############################################
# Determine build information. Version specs
# are specified at the top of the file
#############################################
# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

#set 
if( "${GIT_BRANCH}" MATCHES "master" )
   set(VERSION_TAG "dev")
else()
   set(VERSION_TAG "${GIT_BRANCH}")
endif()

message("VERSION TAG: ${VERSION_TAG}")

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the date
execute_process(
   COMMAND date +%F
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   OUTPUT_VARIABLE BUILD_DATE 
   OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the date
execute_process(
   COMMAND date +%T
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   OUTPUT_VARIABLE BUILD_TIME
   OUTPUT_STRIP_TRAILING_WHITESPACE
)

#############################################
# Generate the version.h file to integrate version
# information.
#############################################
configure_file( 
   "${PROJECT_SOURCE_DIR}/revision.h.in"
   "${CMAKE_INSTALL_PREFIX}/include/mongoapi/revision.h"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

list(APPEND CMAKE_MODULE_PATH ${mongoapi_SOURCE_DIR}/cmake)

find_package(Threads REQUIRED)
find_package(JsonBox CONFIG REQUIRED)
find_package(AquetiTools CONFIG REQUIRED)

find_package(BSON-1.0 REQUIRED)
find_package(BSONCxx REQUIRED)
find_package(MongoC REQUIRED)
find_package(MongoCxx REQUIRED)

include_directories(
    ${MONGOC_INCLUDE_DIR}
    ${MONGOCXX_INCLUDE_DIRS}
    ${BSON_INCLUDE_DIRS}
    ${BSONCXX_INCLUDE_DIRS}
    ${CMAKE_INSTALL_PREFIX}/include/bsoncxx/v_noabi
)

#create hte mongoapi library
add_library(mongoapi STATIC
    mongoapi.cpp 
)
TARGET_LINK_LIBRARIES(mongoapi 
    libaquetitools.a
    JsonBox
    ${MONGOC_LIBRARY} 
    ${MONGOCXX_LIBRARY} 
    ${BSONCXX_LIBRARY} 
    ${BSON_LIBRARIES}
    Threads::Threads
)
list(APPEND TARGET_LIST mongoapi)

if( MAKE_TESTS )
   #Make a dedicated test library
   add_library( mongoapi_test STATIC
       test/MongoAPITest.cpp
   )
   target_link_libraries( mongoapi_test
      mongoapi
   )
   list(APPEND TARGET_LIST mongoapi_test )
   
   #Add MongoAPI test as an executavble
   add_executable( testMongoAPI
      test/testMongoAPI.cpp 
   )
   TARGET_LINK_LIBRARIES(testMongoAPI
      mongoapi_test
   )
   list(APPEND TARGET_LIST testMongoAPI )
endif()

# Doxygen support
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

#############################################
#install library files
# This sections initiates the build of the  components in th TARGET_LIST. 
# Each component
#############################################
configure_file(${CMAKE_SOURCE_DIR}/MongoAPIConfig.cmake.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
               @ONLY)

export(TARGETS ${TARGET_LIST} FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

install(TARGETS ${TARGET_LIST} EXPORT ${PROJECT_NAME}
   RUNTIME DESTINATION bin COMPONENT lib
   LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT lib
   ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT lib
   INCLUDES DESTINATION include/mongoapi
   PUBLIC_HEADER DESTINATION include/mongoapi
)

install(FILES mongoapi.h DESTINATION include/mongoapi)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION lib${LIB_SUFFIX}/cmake)
install(EXPORT ${PROJECT_NAME}
        DESTINATION lib${LIB_SUFFIX}/cmake
        FILE ${PROJECT_NAME}Targets.cmake)
