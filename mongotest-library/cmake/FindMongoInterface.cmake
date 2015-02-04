# - Find MongoInterface
# Find the MongoInterface includes and client library
# This module defines
#  MongoInterface_INCLUDE_DIR, where to find MongoInterface.cpp
#  MongoInterface_LIBRARY, the libraries needed to use MongoInterface.
#  MongoInterface_FOUND, If false, do not try to use MongoInterface.

if(MongoInterface_INCLUDE_DIR AND MongoInterface_LIBRARY)
   set(MongoInterface_FOUND TRUE)

else(MongoInterface_INCLUDE_DIR AND MongoInterface_LIBRARY)

  find_path(MongoInterface_INCLUDE_DIR MongoInterface.h
      /usr/include/
      /usr/local/include/
      /usr/include/MongoInterface/
      /usr/local/include/MongoInterface/
	  /opt/mongo/include/
      $ENV{ProgramFiles}/MongoInterface/*/include
      $ENV{SystemDrive}/MongoInterface/*/include
      )

if(WIN32)
  find_library(MongoInterface_LIBRARY NAMES MongoInterface
      PATHS
      $ENV{ProgramFiles}/MongoInterface/*/lib
      $ENV{SystemDrive}/MongoInterface/*/lib
      )
else(WIN32)
  find_library(MongoInterface_LIBRARY NAMES MongoInterface
      PATHS
      /usr/lib
      /usr/lib/MongoInterface
      /usr/local/lib
      /usr/local/lib/MongoInterface
      /opt/MongoInterface/lib
      )
endif(WIN32)

  if(MongoInterface_INCLUDE_DIR AND MongoInterface_LIBRARY)
    set(MongoInterface_FOUND TRUE)
    message(STATUS "Found MongoInterface: ${MongoInterface_INCLUDE_DIR}, ${MongoInterface_LIBRARIES}")
  else(MongoInterface_INCLUDE_DIR AND MongoInterface_LIBRARY)
    set(MongoInterface_FOUND FALSE)
    if (MongoInterface_FIND_REQUIRED)
		message(FATAL_ERROR "MongoInterface not found.")
	else (MongoInterface_FIND_REQUIRED)
		message(STATUS "MongoInterface not found.")
	endif (MongoInterface_FIND_REQUIRED)
  endif(MongoInterface_INCLUDE_DIR AND MongoInterface_LIBRARY)

  mark_as_advanced(MongoInterface_INCLUDE_DIR MongoInterface_LIBRARY)

endif(MongoInterface_INCLUDE_DIR AND MongoInterface_LIBRARY)
