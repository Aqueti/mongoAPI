CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MongoInterface)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option( USE_SUPERBUILD "Use the superbuild to pull in dependencies" ON)

if( USE_SUPERBUILD)
      option(GIT_HTTPS "Clone git repositories with HTTPS" ON)
      include(ExternalProject)
      include(Superbuild.cmake)
      return()
endif()

# Additional build flags
if(UNIX)
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexpensive-optimizations")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
endif()

list(APPEND CMAKE_MODULE_PATH ${ACI_SOURCE_DIR} ${ACI_SOURCE_DIR}/cmake)


set(INSTALL_DIR "${CMAKE_BINARY_DIR}/INSTALL")


list(APPEND CMAKE_MODULE_PATH ${MongoInterface_SOURCE_DIR} ${MongoInterface_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

FIND_PACKAGE(Boost 1.55 COMPONENTS system regex thread filesystem program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES( ${Threads_INCLUDE_DIR})
FIND_PACKAGE(JSONBOX REQUIRED)
INCLUDE_DIRECTORIES( ${JSONBOX_INCLUDE_DIRS})
FIND_PACKAGE(MongoDB REQUIRED)
INCLUDE_DIRECTORIES( ${MongoDB_INCLUDE_DIR})

add_library(MongoInterface STATIC MongoInterface.cpp MongoInterface.h)
TARGET_LINK_LIBRARIES(MongoInterface ${JSONBOX_LIBRARY_RELEASE} ${MongoDB_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS MongoInterface ARCHIVE DESTINATION "lib" COMPONENT lib)
install(FILES MongoInterface.h DESTINATION include)

#add_executable(MongoInterfaceTest MongoInterface_test.cpp)
#TARGET_LINK_LIBRARIES(MongoInterfaceTest MongoInterface)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ALL
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

#add_subdirectory(tests)

