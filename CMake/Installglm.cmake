include ( "ExternalProject" )

# GLM #####################################
ExternalProject_Add ( Install_glm
    GIT_REPOSITORY
        https://github.com/g-truc/glm
    GIT_SHALLOW
        TRUE
    SOURCE_DIR
        ${CMAKE_BINARY_DIR}/Downloads/glm
    INSTALL_DIR
        ${GLM_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DGLM_TEST_ENABLE_CXX_11=TRUE
        -DGLM_TEST_ENABLE=FALSE
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        ${GLM_CMAKE_FLAGS}
    EXCLUDE_FROM_ALL
        TRUE
    )
