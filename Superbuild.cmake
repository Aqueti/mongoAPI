set(cmake_common_args
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
    -DCMAKE_INCLUDE_PATH:PATH=${CMAKE_INSTALL_PREFIX}/include
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/INSTALL
#    -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_CURRENT_BINARY_DIR}/INSTALL/lib${LIB_SUFFIX}
    -DCMAKE_INCLUDE_PATH:PATH=${CMAKE_CURRENT_BINARY_DIR}/INSTALL/include

    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DBUILD_TESTS:BOOL=${BUILD_TESTS}
    -DUSE_DOXYGEN:BOOL=${USE_DOXYGEN}
    -DBUILD_DEB_PACKAGE:BOOL=${BUILD_DEB_PACKAGE}
    -DUSE_SUPERBUILD:BOOL=OFF
)

# list(APPEND cmake_local_args 
#     ${cmake_common_args}
#     -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
#     -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_BINARY_DIR}/INSTALL/lib${LIB_SUFFIX}
#     -DCMAKE_INCLUDE_PATH:PATH=${CMAKE_BINARY_DIR}/INSTALL/include
#  )

#  list(APPEND cmake_global_args 
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
#    -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
#    -DCMAKE_INCLUDE_PATH:PATH=${CMAKE_INSTALL_PREFIX}/include
#  )

add_custom_target(submodule_init
    COMMAND ${GIT_EXECUTABLE} submodule init ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# if MASTER is true, project will always be built. Else, only built if dependency
macro(add_external_project MYNAME LOCATION MASTER DEPENDS ARGS)
    if(NOT ${MASTER})
        set(EXCLUDE ON)
    else()
        set(EXCLUDE OFF)
    endif(NOT ${MASTER})
    ExternalProject_Add( ${MYNAME}
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/${LOCATION}
        BUILD_ALWAYS 1
        EXCLUDE_FROM_ALL ${EXCLUDE}
        DOWNLOAD_COMMAND ${GIT_EXECUTABLE} submodule update --checkout ${CMAKE_SOURCE_DIR}/${LOCATION}
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}
        CMAKE_ARGS ${cmake_common_args} ${ARGS}
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
        DEPENDS ${DEPENDS} submodule_init
    )
endmacro(add_external_project)

#See if we have all of the packages
if( NOT FORCE_MODULES )
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
  #find_package(libbson-static-1.0 QUIET)
  find_package(libbsoncxx QUIET )
  find_package(libmongoc-static-1.0 QUIET )
  find_package(libmongocxx QUIET )
endif()


#determine what local modules we need to build
if( NOT  libbson_static-1.0_FOUND )
    #set(BUILD_BSON true)
   set(BUILD_MONGOC true)
   set(BUILD_MONGOCXX true)
endif()
if( NOT  libmongoc-static_FOUND )
   set(BUILD_MONGOC true)
   set(BUILD_MONGOCXX true)
endif()
if( libmongocxx_FOUND )
   set(BUILD_MONGOCXX true)
endif()


# Build the missing modules 
# Otherwise, it remains as it was set
#if( BUILD_BSON )
#  set(BSON_ARGS
#    -DENABLE_TESTS:BOOL=OFF
##    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/INSTALL
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
#  )
#  add_external_project(libbson dependencies/libbson OFF "" "${BSON_ARGS}")
#  set(depends libbson)
#endif()
if( BUILD_MONGOC)
  set(MONGOC_ARGS
    -DENABLE_TESTS:BOOL=OFF
    -DENABLE_EXAMPLES:BOOL=OFF
    -DENABLE_SASL:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/INSTALL
  )
  add_external_project(MongoC dependencies/mongo-c-driver OFF "${depends}" "${MONGOC_ARGS}")
  set(depends MongoC)
endif()

if( BUILD_MONGOCXX )
  set(MONGOCXX_ARGS
#    -DLIBMONGOC_DIR=${CMAKE_BINARY_DIR}/INSTALL
#    -DLIBBSON_DIR=${CMAKE_BINARY_DIR}/INSTALL
#    -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR}/INSTALL
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/INSTALL
    -DLIBMONGOC_DIR=${CMAKE_INSTALL_PREFIX}
    #-DLIBBSON_DIR=${CMAKE_INSTALL_PREFIX}
#    -DCMAKE_PREFIX_PATH:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
  )
  add_external_project(MongoCXX dependencies/mongo-cxx-driver OFF "${depends}" "${MONGOCXX_ARGS}")
  set(depends MongoCXX)
endif()

if(USE_SUPERBUILD)
   add_external_project(JsonBox dependencies/JsonBox OFF "" "")
   add_external_project(AquetiTools dependencies/AquetiTools OFF "JsonBox" "")
   set(superdepends AquetiTools)
endif()

ExternalProject_Add(
  mongoapi
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}
  BUILD_ALWAYS 1 
  CMAKE_ARGS
     ${cmake_common_args}
     -DDOXYGEN_DIR=${CMAKE_BINARY_DIR}/INSTALL/Doxygen
    -DBUILD_MODULES:BOOL=false
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  LOG_INSTALL 1
  DEPENDS ${depends}  ${superdepends}
)
