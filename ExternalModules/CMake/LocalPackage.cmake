cmake_minimum_required ( VERSION 3.3 )

# Macros for finding and installing packages

# make sure this module is only included once per project
if ( NOT LOCAL_PACKAGE_FIRST_PROJECT )
    set ( CUSTOM_CMAKE_MODULES_LOCATION ${CMAKE_CURRENT_LIST_DIR} CACHE STRING "Location for LocalPackage to find CMake modules" )
    set ( LOCAL_PACKAGE_FIRST_PROJECT ${PROJECT_NAME} CACHE INTERNAL "" )
    set ( LOCAL_PACKAGE_INSTALL_LOCATION "${CMAKE_BINARY_DIR}/Dependencies" CACHE INTERNAL "Location where to search and install all local packages")
    set ( LOCAL_PACKAGE_USE_SYSTEM TRUE CACHE INTERNAL "Allow system packages to be used" )
    option ( LOCAL_PACKAGE_USE_SYSTEM "Allow system packages to be used" TRUE )
endif()

#message ( STATUS "Master - ${LOCAL_PACKAGE_FIRST_PROJECT} Current - ${PROJECT_NAME} CMake modules - ${CUSTOM_CMAKE_MODULES_LOCATION} Install - ${LOCAL_PACKAGE_INSTALL_LOCATION}")

function ( set_local_package_install_location NEW_PACKAGE_INSTALL_LOCATION )
    if ( "${PROJECT_NAME}" STREQUAL "${LOCAL_PACKAGE_FIRST_PROJECT}" )
        message ( STATUS "Setting local package install location to ${NEW_PACKAGE_INSTALL_LOCATION}" )
        set ( LOCAL_PACKAGE_INSTALL_LOCATION "${NEW_PACKAGE_INSTALL_LOCATION}" CACHE INTERNAL "Location where to search and install all local packages")
    endif()
endfunction()

macro ( find_local_package )
    if ( NOT LOCAL_PACKAGE_ALLOW_SYSTEM )
        #message ( STATUS "Searching for package in ${LOCAL_PACKAGE_INSTALL_LOCATION}" )
        set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
        set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
        set ( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )
        set ( CMAKE_FIND_ROOT_PATH ${LOCAL_PACKAGE_INSTALL_LOCATION} )
        # Search local libraries
        set ( OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} )
        set ( OLD_CMAKE_SEARCH_PATH ${CMAKE_PREFIX_PATH} )
        list ( APPEND CMAKE_MODULE_PATH ${CUSTOM_CMAKE_MODULES_LOCATION} )
        set ( CMAKE_PREFIX_PATH ${LOCAL_PACKAGE_INSTALL_LOCATION} )
    endif()
    find_package ( ${ARGN} )
    if ( NOT LOCAL_PACKAGE_ALLOW_SYSTEM )
        # Before proceeding, restore cmake search logic
        set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
        set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
        set ( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER )
        set ( CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH} )
        set ( CMAKE_SEARCH_PATH ${OLD_CMAKE_PREFIX_PATH} )
    endif()
endmacro()

function ( install_local_package PACKAGE_NAME )
    if ( TARGET "Install_${PACKAGE_NAME}" )
        return()
    endif()
    message ( STATUS "Marking ${PACKAGE_NAME} for installation")

    # In case I want to specify a custom install path for this dependency I can use this. Also specifies the default installation path for it
    string ( TOUPPER ${PACKAGE_NAME} UPPERCASE_PACKAGE_NAME )
    set ( ${UPPERCASE_PACKAGE_NAME}_PATH ${LOCAL_PACKAGE_INSTALL_LOCATION} CACHE INTERNAL "Location where to install ${PACKAGE_NAME}" )
    include ( Install${PACKAGE_NAME} )

    # Append to an existing target, or create a new one
    if ( TARGET BuildLocalPackages )
        add_dependencies ( BuildLocalPackages Install_${PACKAGE_NAME} )
    else()
        add_custom_target ( BuildLocalPackages ALL DEPENDS Install_${PACKAGE_NAME} )
    endif()
endfunction()

macro ( install_local_packages )
    if ( TARGET BuildLocalPackages )
        if ( "${PROJECT_NAME}" STREQUAL "${LOCAL_PACKAGE_FIRST_PROJECT}" )
            message ( STATUS "Installing local packages..." )
        endif()
        return()
    endif()
endmacro()
