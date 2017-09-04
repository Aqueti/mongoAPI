CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)

PROJECT(mongoAPI VERSION "0.1.0.0")

option(USE_SUPERBUILD "Use superbuild to pull in and build dependencies." ON)
option(BUILD_TESTS "Build test libraries and excutables." ON)

#We need a git executable so we can determine versions
find_package(Git REQUIRED)

# Doxygen support
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_DIR "${CMAKE_BINARY_DIR}/Doxygen" CACHE STRING "Where to build Doxygen")
    file(MAKE_DIRECTORY ${DOXYGEN_DIR})
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${DOXYGEN_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${DOXYGEN_DIR}/Doxyfile
        WORKING_DIRECTORY ${DOXYGEN_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)


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
  COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
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
if(NOT ${GIT_BRANCH} MATCHES "${GIT_BRANCH_PREV}")
    set(CONFIGURE ON)
endif()
set(GIT_BRANCH_PREV ${GIT_BRANCH} CACHE STRING "Git Branch" FORCE)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT ${GIT_COMMIT_HASH} MATCHES "${GIT_COMMIT_HASH_PREV}")
    set(CONFIGURE ON)
endif()
set(GIT_COMMIT_HASH_PREV ${GIT_COMMIT_HASH} CACHE STRING "Git Commit Hash" FORCE)


# Get the date
execute_process(
   COMMAND date +%F
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   OUTPUT_VARIABLE BUILD_DATE 
   OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT ${BUILD_DATE} MATCHES "${BUILD_DATE_PREV}")
    set(CONFIGURE ON)
endif()
set(BUILD_DATE_PREV ${BUILD_DATE} CACHE STRING "Build Date" FORCE)


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
if(CONFIGURE OR NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/revision.h)
    configure_file( 
        "${PROJECT_SOURCE_DIR}/revision.h.in"
        "revision.h"
    )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(BSON-1.0 REQUIRED)
find_package(BSONCxx REQUIRED)
find_package(MongoC REQUIRED)
find_package(MongoCxx REQUIRED)
find_package(JsonBox CONFIG REQUIRED)

include_directories(
    ${MONGOC_INCLUDE_DIR}
    ${MONGOCXX_INCLUDE_DIRS}
    ${BSON_INCLUDE_DIRS}
    ${BSONCXX_INCLUDE_DIRS}
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#create hte mongoapi library
add_library(mongoAPI STATIC
    mongoapi.cpp 
)
TARGET_LINK_LIBRARIES(mongoAPI PUBLIC
    JsonBox
    ${MONGOC_LIBRARY} 
    ${MONGOCXX_LIBRARY} 
    ${BSONCXX_LIBRARY} 
    ${BSON_LIBRARIES}
)
set(TARGET_LIST mongoAPI)

set(MONGO_HEADERS mongoapi.h)

if( BUILD_TESTS )
   #Make a dedicated test library
   add_library( mongoAPI_test STATIC
       test/MongoAPITest.cpp
       test/MongoInterfaceTest.cpp
   )
   target_link_libraries( mongoAPI_test
      mongoAPI
   )
   list(APPEND TARGET_LIST mongoAPI_test )

   list(APPEND MONGO_HEADERS test/MongoAPITest.h)
   
   #Add MongoAPI test as an executavble
   add_executable( testMongoAPI
      test/testMongoAPI.cpp 
   )
   target_link_libraries(testMongoAPI
      mongoAPI_test
   )
   list(APPEND TARGET_LIST testMongoAPI )
endif()

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

install(FILES ${MONGO_HEADERS} DESTINATION include/mongoapi)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION lib${LIB_SUFFIX}/cmake)
install(EXPORT ${PROJECT_NAME}
        DESTINATION lib${LIB_SUFFIX}/cmake
        FILE ${PROJECT_NAME}Targets.cmake)
