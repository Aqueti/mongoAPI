CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)

PROJECT(mongoapi VERSION "0.1.0.0")

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
  OUTPUT_VARIABLE ATL_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the date
execute_process(
   COMMAND date +%F
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   OUTPUT_VARIABLE ATL_BUILD_DATE 
   OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the date
execute_process(
   COMMAND date +%T
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   OUTPUT_VARIABLE ATL_BUILD_TIME
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



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

list(APPEND CMAKE_MODULE_PATH ${mongoapi_SOURCE_DIR} ${mongoapi_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

#FIND_PACKAGE(Boost 1.54 COMPONENTS system regex thread filesystem program_options REQUIRED )
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

include_directories( ${CMAKE_INSTALL_PREFIX}/INSTALL/include/bsoncxx/v_noabi )

link_directories( ${CMAKE_INSTALL_PREFIX}/lib )

MESSAGE(STATUS "LIBRARY STATUS:")
#MESSAGE(STATUS "Threads: " ${Threads_INCLUDE_DIR})
MESSAGE(STATUS "JsonBox: " ${JSONBOX_INCLUDE_DIRS})
#MESSAGE(STATUS "MongoDB: " ${MongoDB_INCLUDE_DIR})

#create hte mongoapi library
add_library(mongoapi STATIC mongoapi.cpp )
TARGET_LINK_LIBRARIES(mongoapi 
   ${JSONBOX_LIBRARY_RELEASE} 
   ${MONGOC_LIBRARY} 
   ${MONGOCXX_LIBRARY} 
   ${BSONCXX_LIBRARY} 
   ${CMAKE_THREAD_LIBS_INIT}
   libaquetitools.a
)
list(APPEND TARGET_LIST mongoapi )

#Add MongoAPI test as an executavble
add_executable( MongoAPITest 
   mongotest/MongoInterface_test.cpp 
)
TARGET_LINK_LIBRARIES(MongoAPITest 
   mongoapi 
#   ${JSONBOX_LIBRARY_RELEASE} 
#   ${MONGOC_LIBRARY} 
#   ${MONGOCXX_LIBRARY} 
#   ${BSONCXX_LIBARY} 
#   ${CMAKE_THREAD_LIBS_INIT}
)

list(APPEND TARGET_LIST MongoAPITest )

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
install( TARGETS 
   ${TARGET_LIST}
   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib${LIB_SUFFIX}
   ARCHIVE DESTINATION lib${LIB_SUFFIX}
   INCLUDES DESTINATION include/aqueti
   PUBLIC_HEADER DESTINATION include/aqueti
)

install(FILES mongoapi.h DESTINATION include/mongoapi)
