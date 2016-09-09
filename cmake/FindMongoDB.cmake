# - Find MongoDB
# Find the MongoDB includes and client library
# This module defines where to find bson.h
#
include(FindPackageHandleStandardArgs)

# if BSON_ROOT has defined in system environment, use this as a hint
set(BSON_ROOT "$ENV{BSON_ROOT}")
set(INSTALL_PATH "${CMAKE_BINARY_DIR}/INSTALL")
set(MONGOC_ROOT "$ENV{MONGOC_ROOT}")

#find the header files
find_path(BSON_INCLUDE_DIRS "bson.h" HINTS 
   "${BSON_ROOT}/include"
   "${INSTALL_PATH}/include/libbson-1.0"
)
message( "INSTALL_PATH: ${INSTALL_PATH}")
find_library(BSON_LIBRARY NAMES BSON HINTS 
   "${BSON_ROOT}/lib"
   "${INSTALL_PATH}/lib"
)


# if MONGOC_ROOT has defined in system environment, use this as a hint
set(MONGOC_ROOT "$ENV{MONGOC_ROOT}")
set(INSTALL_PATH "${CMAKE_BINARY_DIR}/INSTALL")

#find the header files
find_path(MONGOC_INCLUDE_DIRS "mongoc.h" HINTS 
   "${MONGOC_ROOT}/include"
   "${INSTALL_PATH}/include/libmongoc-1.0"
)


find_library(MONGOC_LIBRARY NAMES MONGOC HINTS 
   "${MONGOC_ROOT}/lib"
   "${INSTALL_PATH}/lib"
)
#
find_package_handle_standard_args(BSON DEFAULT_MSG BSON_INCLUDE_DIRS BSON_LIBRARIES)
find_package_handle_standard_args(MONGOC DEFAULT_MSG MONGOC_INCLUDE_DIRS MONGOC_LIBRARIES)
