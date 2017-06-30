set(cmake_common_args
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
    -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_BINARY_DIR}/INSTALL/lib${LIB_SUFFIX}
    -DCMAKE_INCLUDE_PATH:PATH=${CMAKE_BINARY_DIR}/INSTALL/include
    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DMAKE_TESTS:BOOL=${MAKE_TESTS}
    -DUSE_DOXYGEN:BOOL=${USE_DOXYGEN}
    -DMAKE_DEB_PACKAGE:BOOL=${MAKE_DEB_PACKAGE}
    -DUSE_SUPERBUILD:BOOL=OFF
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
        DOWNLOAD_COMMAND git submodule update --init --checkout ${CMAKE_SOURCE_DIR}/${LOCATION}
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}
        CMAKE_ARGS ${cmake_common_args} ${ARGS}
        INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
        DEPENDS ${DEPENDS}
    )
endmacro(add_external_project)

set(MONGOC_ARGS
    -DENABLE_TESTS:BOOL=OFF
    -DENABLE_EXAMPLES:BOOL=OFF
    -DENABLE_SASL:BOOL=OFF
)
set(MONGOCXX_ARGS
    -DLIBMONGOC_DIR=${CMAKE_BINARY_DIR}/INSTALL
    -DLIBBSON_DIR=${CMAKE_BINARY_DIR}/INSTALL
    -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR}/INSTALL
)
set(BSON_ARGS
    -DENABLE_TESTS:BOOL=OFF
)

add_external_project(libbson dependencies/libbson OFF "" "${BSON_ARGS}")
add_external_project(MongoC dependencies/mongo-c-driver OFF "libbson" "${MONGOC_ARGS}")
add_external_project(MongoCXX dependencies/mongo-cxx-driver OFF "MongoC;libbson" "${MONGOCXX_ARGS}")
add_external_project(JsonBox dependencies/JsonBox OFF "" "")
add_external_project(AquetiTools dependencies/AquetiTools OFF "JsonBox" "")

ExternalProject_Add (
  mongoapi
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  BUILD_ALWAYS 1 
  CMAKE_ARGS
    ${cmake_common_args}
    -DDOXYGEN_DIR=${CMAKE_BINARY_DIR}/INSTALL/Doxygen
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  DEPENDS JsonBox MongoCXX AquetiTools
)


