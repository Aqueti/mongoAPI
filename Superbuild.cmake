set(cmake_common_args
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/INSTALL/lib"
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

#ExternalProject_Add(
#  MongoDB
#  GIT_REPOSITORY "https://github.com/mongodb/mongo-cxx-driver.git"
#  GIT_TAG "legacy"
#  CONFIGURE_COMMAND 
#  COMMAND echo "building mongo-cxx-driver"
#  BUILD_IN_SOURCE 1 
#  BUILD_COMMAND scons -j 4 --prefix=${CMAKE_BINARY_DIR}/INSTALL install
#  INSTALL_COMMAND 
#  COMMAND echo "built mongo-cxx-driver"
#)


#Mongo C drivers
ExternalProject_Add(
  MongoCDriver
  GIT_REPOSITORY "https://github.com/mongodb/mongo-c-driver"
   GIT_TAG "1.3.5"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ./autogen.sh --with-libbson=bundled --prefix=${CMAKE_BINARY_DIR}/INSTALL
  BUILD_COMMAND make -j
#  INSTALL_COMMAND make DESTDIR=${CMAKE_BINARY_DIR} install 
  INSTALL_COMMAND make install 

)

#Mongo CXX drivers
ExternalProject_Add(
   MongoCxxDriver
   GIT_REPOSITORY "https://github.com/mongodb/mongo-cxx-driver"
#   GIT_TAG "3.1-dev"
   CMAKE_ARGS
     ${cmake_common_args}
     -DLIBBSON_DIR=${CMAKE_BINARY_DIR}/INSTALL
     -DLIBMONGOC_DIR=${CMAKE_BINARY_DIR}/INSTALL
   INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
   DEPENDS MongoCDriver
)

ExternalProject_Add (
  MongoInterface
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  CMAKE_ARGS
    ${cmake_common_args}
    -DUSE_SUPERBUILD:BOOL=OFF
    -DDOXYGEN_DIR=${CMAKE_BINARY_DIR}/INSTALL/Doxygen
  INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
  DEPENDS JsonBox MongoCxxDriver
)
