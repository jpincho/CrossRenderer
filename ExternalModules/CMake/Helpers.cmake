cmake_minimum_required ( VERSION 3.3 )
function ( FindModuleStart PACKAGE_NAME )
    string ( TOUPPER ${PACKAGE_NAME} UPPERCASE_PACKAGE_NAME )
    set ( ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATHS $ENV{${UPPERCASE_PACKAGE_NAME}_PATH} ${${UPPERCASE_PACKAGE_NAME}_PATH} ${LOCAL_PACKAGE_INSTALL_LOCATION} )
    set ( ${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATHS $ENV{${UPPERCASE_PACKAGE_NAME}_PATH} ${${UPPERCASE_PACKAGE_NAME}_PATH} ${LOCAL_PACKAGE_INSTALL_LOCATION} )
    set ( ${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATH_SUFFIXES include )
    set ( ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATH_SUFFIXES lib )

    if ( "ALLOW_SYSTEM_LIBS" IN_LIST ARGN )
        #message ( STATUS "Searching system libs")
        list ( APPEND ${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATHS
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /usr
            )
        list  ( APPEND ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATHS
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /usr
            )

        if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            #message ( STATUS "Searching for x64 libs" )
            list ( APPEND ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATH_SUFFIXES "lib64" "lib/x64" "x64" )
        else()
            #message ( STATUS "Searching for x32 libs" )
            list ( APPEND ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATH_SUFFIXES "lib/x86" "x86" )
        endif()
        find_package(PkgConfig QUIET)

        if ( PKG_CONFIG_FOUND )
            pkg_check_modules ( PKG_CONFIG_${UPPERCASE_PACKAGE_NAME} QUIET ${PACKAGE_NAME} )
            list ( APPEND ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATHS ${PKG_CONFIG_${UPPERCASE_PACKAGE_NAME}_LIBRARY_DIRS} )
            list ( APPEND ${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATHS ${PKG_CONFIG_${UPPERCASE_PACKAGE_NAME}_INCLUDE_DIRS} )
            list ( APPEND ${UPPERCASE_PACKAGE_NAME}_LIBRARY_NAMES ${PKG_CONFIG_${UPPERCASE_PACKAGE_NAME}_LIBRARIES} )
        endif ()
    else()
        set ( ${UPPERCASE_PACKAGE_NAME}_SEARCH_OPTIONS NO_DEFAULT_PATH )
    endif()

    set ( ${UPPERCASE_PACKAGE_NAME}_INCLUDE_DIR ${UPPERCASE_PACKAGE_NAME}_INCLUDE_DIR-NOTFOUND PARENT_SCOPE )
    set ( ${UPPERCASE_PACKAGE_NAME}_LIBRARY ${UPPERCASE_PACKAGE_NAME}_LIBRARY-NOTFOUND PARENT_SCOPE )
    set ( ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATHS ${${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATHS} PARENT_SCOPE )
    set ( ${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATHS ${${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATHS} PARENT_SCOPE )
    set ( ${UPPERCASE_PACKAGE_NAME}_LIBRARY_NAMES ${${UPPERCASE_PACKAGE_NAME}_LIBRARY_NAMES} PARENT_SCOPE )
    set ( ${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATH_SUFFIXES ${${UPPERCASE_PACKAGE_NAME}_INCLUDE_SEARCH_PATH_SUFFIXES} PARENT_SCOPE )
    set ( ${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATH_SUFFIXES ${${UPPERCASE_PACKAGE_NAME}_LIBRARY_SEARCH_PATH_SUFFIXES} PARENT_SCOPE )
#    message ( STATUS "COMPONENTS - ${${UPPERCASE_PACKAGE_NAME}_FIND_COMPONENTS}" )
endfunction()
