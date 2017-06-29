set(cmake_common_args
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
    -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_BINARY_DIR}/INSTALL/lib${LIB_SUFFIX}
    -DUSE_SUPERBUILD:BOOL=FALSE
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
        DOWNLOAD_COMMAND git submodule update --init ${CMAKE_SOURCE_DIR}/${LOCATION}
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}
        CMAKE_ARGS ${cmake_common_args} ${ARGS}
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
        DEPENDS ${DEPENDS}
    )
endmacro(add_external_project)

add_external_project(JsonBox dependencies/JsonBox OFF "" "")
add_external_project(libbson dependencies/libbson OFF "" "")
add_external_project(MongoC dependencies/mongo-c-driver OFF "libbson" "")
add_external_project(MongoCXX dependencies/mongo-cxx-driver OFF "MongoC" "")
add_external_project(AquetiTools dependencies/AquetiTools OFF "" "")

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


