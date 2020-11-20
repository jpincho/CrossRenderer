include ( "ExternalProject" )

ExternalProject_Add ( Install_GLM
    #GIT_REPOSITORY "https://github.com/g-truc/glm"
    URL
        "https://github.com/g-truc/glm/archive/0.9.7.6.zip"
    URL_HASH
        SHA512=53580df63d465a29b011506ee9595eecaf848a996b918418499ad58cb26608ce4566f1266df080ebc0f58c9efca8db8c7dd6371cbe15bf6579d862a53143b431
    DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}/Downloads/
    SOURCE_DIR
        ${CMAKE_BINARY_DIR}/Downloads/GLM
    INSTALL_DIR
        ${GLM_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        ${GLM_CMAKE_FLAGS}
    )
