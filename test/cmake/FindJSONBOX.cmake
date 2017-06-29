include(FindPackageHandleStandardArgs)
  
# if JSONBOX_ROOT has defined in system environment, use this as a hint
set(JSONBOX_ROOT "$ENV{JSONBOX_ROOT}")
  
find_path(JSONBOX_INCLUDE_DIRS "JsonBox.h" HINTS "${JSONBOX_ROOT}/include")
  
find_library(JSONBOX_LIBRARY_RELEASE NAMES JsonBox HINTS "${JSONBOX_ROOT}/lib")
find_library(JSONBOX_LIBRARY_DEBUG NAMES JsonBox_d HINTS "${JSONBOX_ROOT}/lib")
  
if ("${JSONBOX_LIBRARY_DEBUG}" STREQUAL "JSONBOX_LIBRARY_DEBUG-NOTFOUND")
    set(JSONBOX_LIBRARY_DEBUG "${JSONBOX_LIBRARY_RELEASE}")
    message("JSONBOX Warning: Debug library not found, substituting Release library")
endif()
  
set(JSONBOX_LIBRARIES "debug" ${JSONBOX_LIBRARY_DEBUG} "optimized" ${JSONBOX_LIBRARY_RELEASE})
  
find_package_handle_standard_args(JsonBox DEFAULT_MSG JSONBOX_INCLUDE_DIRS JSONBOX_LIBRARIES)

