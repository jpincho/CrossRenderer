include ( "Helpers" )

FindModuleStart( Assimp ALLOW_SYSTEM_LIBS )

#message ( STATUS "Searching for ASSIMP header in ${ASSIMP_INCLUDE_SEARCH_PATHS} with suffixes ${ASSIMP_INCLUDE_SEARCH_PATH_SUFFIXES}")
#message ( STATUS "Searching for ASSIMP library in ${ASSIMP_LIBRARY_SEARCH_PATHS} with suffixes ${ASSIMP_LIBRARY_SEARCH_PATH_SUFFIXES}")

find_path ( ASSIMP_INCLUDE_DIR
    NAMES
        assimp/Importer.hpp
    PATH_SUFFIXES
        ${ASSIMP_INCLUDE_SEARCH_PATH_SUFFIXES}
    PATHS
        ${CMAKE_INCLUDE_PATH}
        ${CMAKE_INSTALL_PREFIX}
        ${ASSIMP_INCLUDE_SEARCH_PATHS}
    )

#message ( STATUS "FindAssimp.cmake - ASSIMP_INCLUDE_DIR - ${ASSIMP_INCLUDE_DIR}")
if ( MSVC12 OR MSVC14 )
    if (MSVC12)
        set ( LIB_NAMES "assimp-vc120-mt" )
        list ( APPEND LIB_NAMES "assimp-vc120-mtd" )
    elseif ( MSVC14 )
        set ( LIB_NAMES "assimp-vc142-mt" )
        list ( APPEND LIB_NAMES "assimp-vc142-mtd" )
    endif()
else()
    set ( LIB_NAMES assimp )
endif()
#message ( STATUS "FindAssimp.cmake - LIB_NAMES - ${LIB_NAMES}")
find_library ( ASSIMP_LIBRARY
    NAMES
        ${LIB_NAMES}
        ${ASSIMP_LIBRARY_NAMES}
    PATH_SUFFIXES
        ${ASSIMP_LIBRARY_SEARCH_PATH_SUFFIXES}
    PATHS
        ${CMAKE_LIBRARY_PATH}
        ${CMAKE_INSTALL_PREFIX}
        ${ASSIMP_LIBRARY_SEARCH_PATHS}
    )
#message ( STATUS "FindAssimp.cmake - ASSIMP_LIBRARY - ${ASSIMP_LIBRARY}")

list ( APPEND ASSIMP_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR} )
list ( APPEND ASSIMP_LIBRARIES ${ASSIMP_LIBRARY} )

if ( ASSIMP_LIBRARY )
    get_filename_component ( OWNING_DIRECTORY ${ASSIMP_LIBRARY} DIRECTORY )
    find_library ( ZLIB_LIBRARY
        NAMES
            zlibstatic zlibstaticd
        PATHS
            ${OWNING_DIRECTORY}
        ${ASSIMP_SEARCH_OPTIONS}
        )
    find_library ( IrrXML_LIBRARY
        NAMES
            IrrXML IrrXMLd
        PATHS
            ${OWNING_DIRECTORY}
        ${ASSIMP_SEARCH_OPTIONS}
        )
    if ( IrrXML_LIBRARY )
        message ( STATUS "Assimp - Using IrrXML" )
        list ( APPEND ASSIMP_LIBRARIES ${IrrXML_LIBRARY} )
    endif()
    if ( ZLIB_LIBRARY )
        message ( STATUS "Assimp - Using zlib" )
        list ( APPEND ASSIMP_LIBRARIES ${ZLIB_LIBRARY} )
    endif()
endif()

include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args ( Assimp DEFAULT_MSG ASSIMP_LIBRARIES ASSIMP_INCLUDE_DIRS )
