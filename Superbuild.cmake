set(cmake_common_args
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DLIB_SUFFIX:STRING=${LIB_SUFFIX}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/INSTALL

)

set(LIB_PATH ${CMAKE_BINARY_DIR}/INSTALL/lib)
set(INCLUDE_PATH ${CMAKE_BINARY_DIR}/INSTALL/include)

#JSONBOX
set(JSONBOX_REPO "https://github.com/anhero/JsonBox.git")
ExternalProject_Add( jsonbox
   GIT_REPOSITORY ${JSONBOX_REPO}
   CMAKE_ARGS ${cmake_common_args}
   INSTALL_DIR ${CMAKE_BINARY_DIR}/INSTALL
)

