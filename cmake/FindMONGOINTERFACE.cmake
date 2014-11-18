# - Find MongoInterface
# Find the MongoInterface includes and client library
# This module defines
#  MONGOINTERFACE_INCLUDE_DIR, where to find MongoInterface.cpp
#  MONGOINTERFACE_LIBRARY, the libraries needed to use MongoInterface.
#  MONGOINTERFACE_FOUND, If false, do not try to use MongoInterface.

if(MONGOINTERFACE_INCLUDE_DIR AND MONGOINTERFACE_LIBRARY)
   set(MONGOINTERFACE_FOUND TRUE)

else(MONGOINTERFACE_INCLUDE_DIR AND MONGOINTERFACE_LIBRARY)

  find_path(MONGOINTERFACE_INCLUDE_DIR MongoInterface.h
      /usr/include/
      /usr/local/include/
      /usr/include/MongoInterface/
      /usr/local/include/MongoInterface/
	  /opt/mongo/include/
      $ENV{ProgramFiles}/MongoInterface/*/include
      $ENV{SystemDrive}/MongoInterface/*/include
      )

if(WIN32)
  find_library(MONGOINTERFACE_LIBRARY NAMES MONGOINTERFACE
      PATHS
      $ENV{ProgramFiles}/MongoInterface/*/lib
      $ENV{SystemDrive}/MongoInterface/*/lib
      )
else(WIN32)
  find_library(MONGOINTERFACE_LIBRARY NAMES MONGOINTERFACE
      PATHS
      /usr/lib
      /usr/lib/MongoInterface
      /usr/local/lib
      /usr/local/lib/MongoInterface
      /opt/MongoInterface/lib
      )
endif(WIN32)

  if(MONGOINTERFACE_INCLUDE_DIR AND MONGOINTERFACE_LIBRARY)
    set(MONGOINTERFACE_FOUND TRUE)
    message(STATUS "Found MongoInterface: ${MONGOINTERFACE_INCLUDE_DIR}, ${MONGOINTERFACE_LIBRARIES}")
  else(MONGOINTERFACE_INCLUDE_DIR AND MONGOINTERFACE_LIBRARY)
    set(MONGOINTERFACE_FOUND FALSE)
    if (MONGOINTERFACE_FIND_REQUIRED)
		message(FATAL_ERROR "MongoInterface not found.")
	else (MONGOINTERFACE_FIND_REQUIRED)
		message(STATUS "MongoInterface not found.")
	endif (MONGOINTERFACE_FIND_REQUIRED)
  endif(MONGOINTERFACE_INCLUDE_DIR AND MONGOINTERFACE_LIBRARY)

  mark_as_advanced(MONGOINTERFACE_INCLUDE_DIR MONGOINTERFACE_LIBRARY)

endif(MONGOINTERFACE_INCLUDE_DIR AND MONGOINTERFACE_LIBRARY)
