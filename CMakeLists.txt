project ( CrossRenderer )
cmake_minimum_required ( VERSION 3.1 )

set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin" )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin" )

list ( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/ExternalModules/CMake )
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/cmake )

include ( LocalPackage )
include ( target_warning_options )

set ( LOCAL_PACKAGE_ALLOW_SYSTEM TRUE )
set ( LOCAL_PACKAGE_INSTALL_LOCATION "${PROJECT_BINARY_DIR}/output/" )

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
    option ( CROSS_RENDERER_WINDOWS_BACKEND_SUPPORT "Build native Windows backend support" TRUE )
elseif ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
    message ( STATUS "${PROJECT_NAME} - Building for Linux")
endif()

###### DEPENDENCY INSTALLATION ######
# GLM
find_local_package ( glm QUIET )
# In case it gets found by glmConfig, glm_DIR should point to that file
if ( NOT GLM_FOUND AND NOT glm_DIR )
    install_local_package ( glm )
endif()

# SDL2
if ( CROSS_RENDERER_WINDOWS_BACKEND_SUPPORT )
    option ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT "Build SDL2 window backend support" FALSE )
else()
    set ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT TRUE )
endif()

if ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
    find_local_package ( SDL2 QUIET )
    if ( NOT SDL2_FOUND )
        install_local_package ( SDL2 )
    endif()
endif()

find_local_package ( Assimp QUIET )
if ( NOT ASSIMP_FOUND )
    install_local_package ( Assimp )
endif()
install_local_packages()

###### Window manager backends ######
# SDL2
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

# Windows
if ( CROSS_RENDERER_WINDOWS_BACKEND_SUPPORT )
    message ( STATUS "${PROJECT_NAME} - Building with Windows backend support")
    file ( GLOB CROSS_RENDERER_WINDOWS_SOURCES "WindowManager/Windows/*" )
    list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_WINDOWS_SOURCES} )
    source_group ( "WindowManager/Windows" FILES ${CROSS_RENDERER_WINDOWS_SOURCES} )
endif()

###### Rendering backends ######
# OpenGL backend
set ( OpenGL_GL_PREFERENCE GLVND )
find_package ( OpenGL QUIET )
if ( OPENGL_FOUND )
    option ( CROSS_RENDERER_OPENGL_CORE_SUPPORT "Enable support for OpenGL Core" TRUE )
    if ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
        message ( STATUS "${PROJECT_NAME} - Building OpenGL core backend" )
        file ( GLOB CROSS_RENDERER_OPENGL_SOURCES "OpenGL/*.cpp" "OpenGL/*.h" )
        list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_OPENGL_SOURCES} )
        source_group ( "OpenGL" FILES ${CROSS_RENDERER_OPENGL_SOURCES} )

        # GLAD core
        file ( GLOB_RECURSE GLAD_SOURCES "OpenGL/glad_core/*")
        add_library ( GLAD ${GLAD_SOURCES} )
        target_include_directories ( GLAD PUBLIC ${CMAKE_CURRENT_LIST_DIR}/OpenGL/glad_core/include )
        list ( APPEND CROSS_RENDERER_LIBRARIES GLAD )
        target_warning_options ( GLAD DISABLED )

        if ( CROSS_RENDERER_WINDOWS_BACKEND_SUPPORT )
            list ( APPEND CROSS_RENDERER_LIBRARIES OPENGL32 )

            # GLAD WGL
            file ( GLOB_RECURSE GLAD_WGL_SOURCES "OpenGL/WGL/glad_wgl/*")
            add_library ( GLAD_WGL ${GLAD_WGL_SOURCES} )
            target_include_directories ( GLAD_WGL PUBLIC ${CMAKE_CURRENT_LIST_DIR}/OpenGL/WGL/glad_wgl/include ${CMAKE_CURRENT_LIST_DIR}/OpenGL/glad_core/include )
            list ( APPEND CROSS_RENDERER_LIBRARIES GLAD_WGL )
            target_warning_options ( GLAD_WGL DISABLED )

            # WGL
            file ( GLOB CROSS_RENDERER_WGL_SOURCES "OpenGL/WGL/*.cpp" "OpenGL/WGL/*.h" )
            list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_WGL_SOURCES} )
            source_group ( "OpenGL" FILES ${CROSS_RENDERER_WGL_SOURCES} )
        endif()

        if ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
            # SDL2 + OpenGL
            file ( GLOB CROSS_RENDERER_SDL2_OPENGL_SOURCES "OpenGL/SDL2/*.cpp" "OpenGL/SDL2/*.h" )
            list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_SDL2_OPENGL_SOURCES} )
            source_group ( "OpenGL" FILES ${CROSS_RENDERER_SDL2_OPENGL_SOURCES} )
        endif()
        if ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
            list ( APPEND CROSS_RENDERER_LIBRARIES dl )
        endif()
    endif()
endif()

add_subdirectory ( ExternalModules/Logger )

# Main library
configure_file ( CrossRendererConfig.h.in CrossRendererConfig.h )
file ( GLOB SOURCES "*.cpp" "*.h" "WindowManager/*.cpp" "WindowManager/*.h" CrossRendererConfig.h.in AstyleConfig.astylerc )
list ( APPEND CROSS_RENDERER_SOURCE_FILES ${SOURCES} )
add_library ( CrossRenderer ${CROSS_RENDERER_SOURCE_FILES} )
target_include_directories ( CrossRenderer PUBLIC ${CROSS_RENDERER_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${GLM_INCLUDE_DIRS} ExternalModules )
target_link_libraries ( CrossRenderer PUBLIC ${CROSS_RENDERER_LIBRARIES} Logger )
target_compile_definitions ( CrossRenderer PRIVATE ${CROSS_RENDERER_DEFINITIONS} )
target_warning_options ( CrossRenderer HIGH AS_ERRORS )

# Tests
message ( STATUS "Building imgui" )
file ( GLOB IMGUI_SOURCES ExternalModules/imgui/* )
add_library ( imgui ${IMGUI_SOURCES} )

file ( GLOB ALL_TESTS "Tests/*" )
file ( GLOB COMMON_TEST_SOURCES Tests/*.cpp Tests/*.h )
source_group ( "Common test sources" FILES ${COMMON_TEST_SOURCES} )
foreach ( TEST_ITERATOR ${ALL_TESTS} )
    if ( NOT IS_DIRECTORY ${TEST_ITERATOR} )
        continue()
    endif()
    get_filename_component ( TEST_NAME ${TEST_ITERATOR} NAME )
    if ( TEST_NAME STREQUAL "Data" )
        continue()
    endif()
    file ( GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES FALSE ${TEST_ITERATOR}/* )
    file ( GLOB_RECURSE OPENGL_CORE_SHADER_SOURCES ${TEST_ITERATOR}/OpenGLCore/* )
    source_group ( "Shaders\\OpenGLCore" FILES ${OPENGL_CORE_SHADER_SOURCES} )
    add_executable ( CrossRendererTest_${TEST_NAME} ${TEST_SOURCES} ${COMMON_TEST_SOURCES} )
    target_link_libraries ( CrossRendererTest_${TEST_NAME} CrossRenderer ${ASSIMP_LIBRARIES} imgui )
    target_include_directories ( CrossRendererTest_${TEST_NAME} PRIVATE ${ASSIMP_INCLUDE_DIRS} )
    target_compile_definitions ( CrossRendererTest_${TEST_NAME} PRIVATE TEST_SOURCE_LOCATION="${CMAKE_CURRENT_LIST_DIR}/Tests/" TEST_NAME="${TEST_NAME}")
    target_warning_options ( CrossRendererTest_${TEST_NAME} HIGH )
    add_test ( CrossRendererTest_${TEST_NAME} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/CrossRendererTest_${TEST_NAME} )
endforeach()
