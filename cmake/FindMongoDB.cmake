# - Find MongoDB
# Find the MongoDB includes and client library
# This module defines
#  MongoDB_INCLUDE_DIR, where to find mongo/client/dbclient.h
#  MongoDB_LIBRARIES, the libraries needed to use MongoDB.
#  MongoDB_FOUND, If false, do not try to use MongoDB.
include(FindPackageHandleStandardArgs)
set(MongoDB_ROOT "$ENV{JSONBOX_ROOT}")

#find_path(MongoDB_INCLUDE_DIR NAMES "mongo" PATHS "${CMAKE_INSTALL_PREFIX}")
set(MongoDB_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

find_library(MongoDB_LIBRARY NAMES mongoclient HINTS "${CMAKE_INSTALL_PREFIX}")
#set(MongoDB_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/include)

message("MongoDB_ROOT: ${CMAKE_INSTALL_PREFIX}")
message("MongoDB_ROOT: ${MongoDB_INCLUDE_DIR}")
message("MongoDB_ROOT: ${MongoDB_LIBRARY}")

find_package_handle_standard_args(MongoDB DEFAULT_MSG MongoDB_INCLUDE_DIR MongoDB_LIBRARY)


#
#
#if(MongoDB_INCLUDE_DIR AND MongoDB_LIBRARIES)
#   set(MongoDB_FOUND TRUE)
#
#else(MongoDB_INCLUDE_DIR AND MongoDB_LIBRARIES)
#
#find_path(MongoDB_INCLUDE_DIR mongo/client/dbclient.h
#      PATHS ${MongoDB_SOURCE_DIR})
#
##      /usr/include/
##      /usr/local/include/
##      /usr/include/mongo/
##      /usr/local/include/mongo/
##	  /opt/mongo/include/
##      $ENV{ProgramFiles}/Mongo/*/include
##      $ENV{SystemDrive}/Mongo/*/include
##      )
#
#MESSAGE(STATUS "------------------------------------------------------------MongoDB INCLUDE---: " ${MongoDB_INCLUDE_DIR})
#
#
#if(WIN32)
#  find_library(MongoDB_LIBRARIES NAMES mongoclient
#      PATHS 
#      ${CMAKE_BINARY_DIR}/INSTALL/lib
#
##      $ENV{ProgramFiles}/Mongo/*/lib
##      $ENV{SystemDrive}/Mongo/*/lib
#      )
#else(WIN32)
#  find_library(MongoDB_LIBRARIES NAMES mongoclient
#      PATHS
#      ${CMAKE_BINARY_DIR}/INSTALL/lib
#      
##      /usr/lib
##      /usr/lib/mongo
##      /usr/local/lib
##      /usr/local/lib/mongo
##	  /opt/mongo/lib
#      )
#endif(WIN32)
#
#  if(MongoDB_INCLUDE_DIR AND MongoDB_LIBRARIES)
#    set(MongoDB_FOUND TRUE)
#    message(STATUS "Found MongoDB: ${MongoDB_INCLUDE_DIR}, ${MongoDB_LIBRARIES}")
#  else(MongoDB_INCLUDE_DIR AND MongoDB_LIBRARIES)
#    set(MongoDB_FOUND FALSE)
#    if (MongoDB_FIND_REQUIRED)
#		message(FATAL_ERROR "MongoDB not found.")
#	else (MongoDB_FIND_REQUIRED)
#		message(STATUS "MongoDB not found.")
#	endif (MongoDB_FIND_REQUIRED)
#  endif(MongoDB_INCLUDE_DIR AND MongoDB_LIBRARIES)
#
#  mark_as_advanced(MongoDB_INCLUDE_DIR MongoDB_LIBRARIES)
#
#endif(MongoDB_INCLUDE_DIR AND MongoDB_LIBRARIES)
