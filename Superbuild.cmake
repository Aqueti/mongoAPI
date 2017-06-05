set(cmake_common_args
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
    -DCMAKE_INSTALL_RPATH:PATH=${CMAKE_BINARY_DIR}/INSTALL/lib${LIB_SUFFIX}
)

ExternalProject_Add(
  JsonBox
  GIT_REPOSITORY "https://github.com/anhero/JsonBox.git"
  GIT_TAG "0.6.2"
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
)

ExternalProject_Add(
  libbson
  GIT_REPOSITORY "https://github.com/mongodb/libbson.git"
  GIT_TAG "1.4.2"
  CMAKE_ARGS ${cmake_common_args}
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
)

ExternalProject_Add(
  MongoC
  GIT_REPOSITORY "https://github.com/mongodb/mongo-c-driver.git"
  GIT_TAG "1.4.2"
  CMAKE_ARGS ${cmake_common_args}
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
  DEPENDS libbson
)

ExternalProject_Add(
  MongoDB
  GIT_REPOSITORY "https://github.com/mongodb/mongo-cxx-driver.git"
  GIT_TAG "r3.0.3"
  CMAKE_ARGS
    ${cmake_common_args}
    -DLIBMONGOC_DIR=${CMAKE_BINARY_DIR}/INSTALL
    -DLIBBSON_DIR=${CMAKE_BINARY_DIR}/INSTALL
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
  DEPENDS MongoC libbson
)

message("AquetToolsDir: ${CMAKE_BINARY_DIR}/INSTALL")
ExternalProject_Add( 
    AquetiTools
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/AquetiTools
#    INSTALL_COMMAND ""
    BUILD_ALWAYS 1 
    CMAKE_ARGS
        ${cmake_common_args}
    INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
    DEPENDS JsonBox
)

ExternalProject_Add (
  mongoapi
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  BUILD_ALWAYS 1 
  CMAKE_ARGS
    ${cmake_common_args}
    -DUSE_SUPERBUILD:BOOL=OFF
    -DDOXYGEN_DIR=${CMAKE_BINARY_DIR}/INSTALL/Doxygen
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
  DEPENDS JsonBox MongoDB
)


