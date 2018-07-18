
cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.2)


PROJECT(mongoAPI VERSION "0.1.0.0")

option(BUILD_MODULES "Build submodules" ON )
option(USE_SUPERBUILD "Use superbuild to pull in and build dependencies." ON)
option(BUILD_TESTS "Build test libraries and excutables." ON)
option(BUILD_TOOLS "Build interface tools." ON)
option(FORCE_MODULES "Force build submodules" OFF)
#option(BUILD_MODULES "Build submodules." CACHE ON)

set(MONGOAPI_BUILD_DIR ${CMAKE_BINARY_DIR})

if(USE_SUPERBUILD)
#   set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/INSTALL)
   set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/INSTALL)
   set(BUILD_MODULES ON)
endif()


message("CURRENT: ${CMAKE_CURRENT_BINARY_DIR}")
message("BINARY: ${CMAKE_BINARY_DIR}")


set( libbsoncxx_DIR ${CMAKE_CURRENT_BINARY_DIR}/INSTALL/lib/cmake/libbsoncxx-3.2.0)
set( libmongocxx_DIR ${CMAKE_CURRENT_BINARY_DIR}/INSTALL/lib/cmake/libmongocxx-3.2.0)
list(APPEND CMAKE_MODULE_PATH 
#   ${CMAKE_CURRENT_SOURCE_DIR}/cmake
   ${CMAKE_INSTALL_PREFIX}/cmake
   ${CMAKE_INSTALL_PREFIX}/lib/cmake
#   ${CMAKE_CURRENT_BINARY_DIR}/INSTALL/cmake
#   ${CMAKE_CURRENT_BINARY_DIR}/INSTALL/lib/cmake
#   ${CMAKE_CURRENT_BINARY_DIR}/INSTALL/lib/cmake
)


if( FORCE_MODULES ) 
   set(BUILD_MODULES ON)
endif()

#We need a git executable so we can determine versions
find_package(Git REQUIRED)

# MongoC++ uses boost and doesn't export the include directory when we're running
# using the MSVC compiler, so we look for it here as well, but it only does this for Windows
if (MSVC)
  find_package(Boost 1.56.0 REQUIRED)
endif()

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
include_directories(
#   ${CMAKE_BINARY_DIR}/INSTALL/include
   ${CMAKE_INSTALL_PREFIX}/include
)

# Superbuild rules
if(BUILD_MODULES)
    include(ExternalProject)
    include(Superbuild.cmake)
    return()
endif()

list(APPEND CMAKE_MODULE_PATH 
   ${CMAKE_INSTALL_PREFIX}/cmake
   ${CMAKE_INSTALL_PREFIX}/lib/cmake
#   ${CMAKE_BINARY_DIR}/INSTALL/cmake
#   ${CMAKE_BINARY_DIR}/INSTALL/lib/cmake
   ${CMAKE_SOURCE_DIR}/INSTALL/lib/cmake
)

list(APPEND CMAKE_PREFIX_PATH
    ${CMAKE_MODULE_PATH}
)

link_directories(
   ${CMAKE_INSTALL_PREFIX}/lib
)

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

find_package(libbson-static-1.0 CONFIG REQUIRED)
find_package(libbsoncxx CONFIG REQUIRED)
find_package(libmongoc-static-1.0 CONFIG REQUIRED)
find_package(libmongocxx CONFIG REQUIRED)
find_package(JsonBox CONFIG REQUIRED)
#find_package(AquetiTools REQUIRED)

link_directories(${LIBMONGOCXX_LIBRARY_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#create the mongoapi library
add_library(MongoAPI STATIC
	mongoapi.cpp
#    MongoInterface.cpp
#    MongoAPI.cpp 
    MongoDatabaseClient.cpp
)

#target_include_directories(MongoAPI
#    PUBLIC ${CMAKE_BINARY_DIR}/INSTALL/include/
#    PUBLIC ${MONGOC_INCLUDE_DIRS} #TODO: This needs to be private
#    PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS}/mongocxx
#    PUBLIC ${BSON_INCLUDE_DIRS}
#     PUBLIC ${LIBBSONCXX_INCLUDE_DIRS}
#    PUBLIC ${CMAKE_INSTALL_PREFIX}/include
#)


include_directories( 
#    ${CMAKE_BINARY_DIR}/INSTALL/include/
    ${MONGOC_INCLUDE_DIRS} 
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${BSON_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
    ${CMAKE_INSTALL_PREFIX}/include
)

if (MSVC)
  target_include_directories(MongoAPI
    PUBLIC ${Boost_INCLUDE_DIRS}
  )
  # Don't let Windows.h define min() and max() as macros; they conflict with our definitions.
  target_compile_definitions(MongoAPI PUBLIC -DNOMINMAX)
endif()

# Convert the library-base-name entries into full paths so that
# libraries and programs that link to us know where to find them.
# We only want the static libraries, so we need to search for the
# names they might have on each system we compile on to avoid
# finding installed .so system libraries.
find_library(MONGOCXX_LIBRARY_PATH NAMES libmongocxx.a mongocxx.lib)
find_library(BSONCXX_LIBRARY_PATH NAMES libbsoncxx.a bsoncxx.lib)
list(APPEND mongoAPI_linkLIBS
#	${CMAKE_INSTALL_PREFIX}/lib/libmongocxx.a
#	${CMAKE_INSTALL_PREFIX}/lib/libbsoncxx.a
    ${MONGOCXX_LIBRARY_PATH} 
    ${BSONCXX_LIBRARY_PATH}
    ${MONGOC_STATIC_LIBRARIES} 
    ${BSON_STATIC_LIBRARIES}
)
	

TARGET_LINK_LIBRARIES(MongoAPI
    PUBLIC AquetiTools
    PUBLIC JsonBox
    PRIVATE ${mongoAPI_linkLIBS}
)
#    INTERFACE ${LIBMONGOCXX_LIBRARIES} 
#    PRIVATE ${LIBBSONCXX_LIBRARIES}
#    PRIVATE ${MONGOC_STATIC_LIBRARIES} 
#    PRIVATE ${BSON_STATIC_LIBRARIES}
#)
list(APPEND TARGET_LIST MongoAPI)

#set(MONGO_HEADERS mongoapi.h MongoDatabaseClient.h)
set(MONGO_HEADERS 
    #MongoAPI.h
   mongoapi.h
   MongoDatabaseClient.h
)

if( FALSE AND BUILD_TESTS )
   #Make a dedicated test library
   add_library( mongoAPI_test STATIC
       test/MongoAPITest.cpp
       test/MongoInterfaceTest.cpp
   )
   target_link_libraries( mongoAPI_test PUBLIC
      MongoAPI
   )
   list(APPEND TARGET_LIST mongoAPI_test )

#   list(APPEND MONGO_HEADERS test/MongoAPITest.h)
   
   #Add MongoAPI test as an executavble
   add_executable( testMongoAPI
      test/testMongoAPI.cpp 
   )
   target_link_libraries(testMongoAPI
      mongoAPI_test
      MongoAPI
   )
   list(APPEND TARGET_LIST testMongoAPI )

   add_executable( MongoTest 
     test/MongoTest.cpp
   )
   TARGET_LINK_LIBRARIES(MongoTest
      MongoAPI
   )
   list(APPEND TARGET_LIST MongoTest )
endif()

#if( BUILD_TOOLS ) 
#   #Add MongoAPI test as an executavble
#   add_executable( mongoTool
#      tools/MongoTool.cpp 
#   )
#   target_link_libraries( mongoTool
#      MongoAPI
#   )
#   list(APPEND TARGET_LIST mongoTool )
#endif( BUILD_TOOLS ) 

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
install(DIRECTORY ${CMAKE_SOURCE_DIR}/cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX})
