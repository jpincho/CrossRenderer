project ( CrossRenderer )
cmake_minimum_required ( VERSION 3.1 )

set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin" )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin" )

list ( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMakeUtils )
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/cmake )

include ( LocalPackage )
include ( target_warning_options )

set ( LOCAL_PACKAGE_ALLOW_SYSTEM TRUE )
set_local_package_install_location ( "${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/${CMAKE_BUILD_TYPE}" )

string ( TOUPPER ${CMAKE_SYSTEM_NAME} UPPERCASE_SYSTEM_NAME )
set ( CROSS_RENDERER_HOST_PLATFORM_${UPPERCASE_SYSTEM_NAME} TRUE )
set ( CROSS_RENDERER_TARGET_PLATFORM_${UPPERCASE_SYSTEM_NAME} TRUE )

if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set ( CROSS_RENDERER_COMPILER_CLANG TRUE )
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    set ( CROSS_RENDERER_COMPILER_GNU TRUE )
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    set ( CROSS_RENDERER_COMPILER_MSVC TRUE )
endif()

message ( STATUS "${PROJECT_NAME} - Building under ${CMAKE_SYSTEM_NAME}")
if ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
    message ( STATUS "${PROJECT_NAME} - Building for Windows")
elseif ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
    message ( STATUS "${PROJECT_NAME} - Building for Linux")
endif()

# GLM
find_local_package ( glm QUIET )
# In case it gets found by glmConfig, glm_DIR should point to that file
if ( NOT GLM_FOUND AND NOT glm_DIR )
    install_local_package ( glm )
endif()

find_local_package ( SDL2 QUIET )
if ( NOT SDL2_FOUND )
    install_local_package ( SDL2 )
endif()
install_local_packages()

# SDL2
find_local_package ( SDL2 QUIET )
if ( SDL2_FOUND )
    option ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT "Build SDL2 window backend support" TRUE )
    if ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
        message ( STATUS "${PROJECT_NAME} - Building with SDL2 backend support" )
        list ( APPEND CROSS_RENDERER_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} )
        list ( APPEND CROSS_RENDERER_LIBRARIES ${SDL2_LIBRARIES} )
        file ( GLOB CROSS_RENDERER_SDL2_SOURCES "WindowManager/SDL2/*" )
        list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_SDL2_SOURCES} )
        source_group ( "WindowManager/SDL2" FILES ${CROSS_RENDERER_SDL2_SOURCES} )
        if ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
            list ( APPEND CROSS_RENDERER_LIBRARIES winmm imm32 version )
        endif()
    endif()
endif()

# OpenGL backend
set ( OpenGL_GL_PREFERENCE GLVND )
find_package ( OpenGL QUIET )
if ( OPENGL_FOUND )
    option ( CROSS_RENDERER_OPENGL_CORE_SUPPORT "Enable support for OpenGL Core" TRUE )
    if ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
        message ( STATUS "${PROJECT_NAME} - Building OpenGL core backend" )
        file ( GLOB CROSS_RENDERER_OPENGL_SOURCES "OpenGL/*.cpp" "OpenGL/*.hpp" )
        list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_OPENGL_SOURCES} )
        source_group ( "OpenGL" FILES ${CROSS_RENDERER_OPENGL_SOURCES} )

        # GLAD core
        file ( GLOB_RECURSE GLAD_SOURCES "OpenGL/glad_core/*")
        add_library ( GLAD ${GLAD_SOURCES} )
        target_include_directories ( GLAD PUBLIC ${CMAKE_CURRENT_LIST_DIR}/OpenGL/glad_core/include )
        list ( APPEND CROSS_RENDERER_LIBRARIES GLAD )
        target_warning_options ( GLAD DISABLED )

        if ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
            # SDL2 + OpenGL
            file ( GLOB CROSS_RENDERER_SDL2_OPENGL_SOURCES "OpenGL/SDL2/*.cpp" "OpenGL/SDL2/*.hpp" )
            list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_SDL2_OPENGL_SOURCES} )
            source_group ( "OpenGL" FILES ${CROSS_RENDERER_SDL2_OPENGL_SOURCES} )
        endif()
        if ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
            list ( APPEND CROSS_RENDERER_LIBRARIES dl )
        endif()
    endif()
endif()

add_subdirectory(cpplib)

# Main library
configure_file ( CrossRendererConfig.hpp.in CrossRendererConfig.hpp )
file ( GLOB SOURCES "*.cpp" "*.hpp" "WindowManager/*.cpp" "WindowManager/*.hpp" CrossRendererConfig.hpp.in AstyleConfig.astylerc )
list ( APPEND CROSS_RENDERER_SOURCE_FILES ${SOURCES} )
add_library ( CrossRenderer ${CROSS_RENDERER_SOURCE_FILES} )
target_include_directories ( CrossRenderer PUBLIC ${CROSS_RENDERER_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${GLM_INCLUDE_DIRS} ExternalModules )
target_link_libraries ( CrossRenderer PUBLIC ${CROSS_RENDERER_LIBRARIES} )
target_compile_definitions ( CrossRenderer PRIVATE ${CROSS_RENDERER_DEFINITIONS} )
target_warning_options ( CrossRenderer HIGH AS_ERRORS )
