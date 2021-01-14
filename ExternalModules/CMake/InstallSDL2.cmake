include ( "ExternalProject" )

# SDL2 ##########################################
if ( WIN32 )
    set ( EXTRA_SDL2_OPTIONS "-DVIDEO_OPENGLES=0" )
endif()
ExternalProject_Add ( Install_SDL2
    URL 
        "https://www.libsdl.org/release/SDL2-2.0.5.zip"
    URL_HASH
        SHA512=19ec2cb5b73b9f67677bae7855d74ed34313b0ff20f978de203e44008734e05f49be3fdcd81b4345c83d48173586e58707828c1d31098e54966ca687c0aa26e5
    DOWNLOAD_DIR
        ${CMAKE_BINARY_DIR}/Downloads/
    SOURCE_DIR
        ${CMAKE_BINARY_DIR}/Downloads/SDL2
    INSTALL_DIR
        ${SDL2_PATH}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DSDL_STATIC=FALSE ${EXTRA_SDL2_OPTIONS}
    EXCLUDE_FROM_ALL
    )

if ( APPLE )
    ExternalProject_Add_Step ( Install_SDL2 Installation # Names of project and custom step
        COMMENT "Copying ${SDL2_PATH}/lib/libSDL2-2.0.1.dylib to binary dir"     # Text printed when step executes
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_PATH}/lib/libSDL2-2.0.1.dylib ${SDL2_PATH}/bin
        DEPENDEES install    # Steps on which this step depends
        )
endif()
