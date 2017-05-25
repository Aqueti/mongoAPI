set(cmake_common_args
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
)

ExternalProject_Add(
  JsonBox
  GIT_REPOSITORY "https://github.com/anhero/JsonBox.git"
  GIT_TAG "0.4.4"
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
)

ExternalProject_Add(
  MongoDB
  GIT_REPOSITORY "https://github.com/mongodb/mongo-cxx-driver.git"
  GIT_TAG "legacy"
  CONFIGURE_COMMAND 
  COMMAND echo "building mongo-cxx-driver"
  BUILD_IN_SOURCE 1 
  BUILD_COMMAND scons -j 4 --disable-warnings-as-errors --prefix=${CMAKE_BINARY_DIR}/INSTALL install
  INSTALL_COMMAND 
  COMMAND echo "built mongo-cxx-driver"
)

ExternalProject_Add (
  MongoInterface
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  CMAKE_ARGS
    ${cmake_common_args}
    -DUSE_SUPERBUILD:BOOL=OFF
    -DDOXYGEN_DIR=${CMAKE_BINARY_DIR}/INSTALL/Doxygen
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
  DEPENDS JsonBox MongoDB
)
