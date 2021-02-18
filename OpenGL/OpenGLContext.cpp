#include <CrossRendererConfig.h>
#include <glad/glad.h>
#if defined ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
#include "SDL2/SDL2GLContext.h"
#endif
#include "../Logger.h"
#include <algorithm>

namespace CrossRenderer
{
namespace OpenGL
{
OpenGLContextInterface *GetOpenGLContext ( const WindowManagerBackend DesiredType )
    {
    switch ( DesiredType )
        {
#if defined ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
        case WindowManagerBackend::SDL2:
            return new SDL2OpenGLContext;
#endif
        default:
            return nullptr;
        }
    }

Version OpenGLContextInterface::GetOpenGLVersion ( void ) const
    {
    return OpenGLVersion;
    }

Version OpenGLContextInterface::GetGLSLVersion ( void ) const
    {
    return GLSLVersion;
    }

bool OpenGLContextInterface::IsExtensionSupported ( const char *Extension )
    {
    return std::find ( Extensions.begin (), Extensions.end (), Extension ) != Extensions.end ();
    }

bool OpenGLContextInterface::DetectOpenGLInformation ( void )
    {
    // MAJOR & MINOR only introduced in GL {,ES} 3.0
    GLint TempMajor = 0, TempMinor = 0;
    glGetIntegerv ( GL_MAJOR_VERSION, &TempMajor );
    glGetIntegerv ( GL_MINOR_VERSION, &TempMinor );
    if ( ( glGetError () == GL_NO_ERROR ) && ( TempMajor != 0 ) )
        {
        OpenGLVersion.Set ( static_cast <unsigned> ( TempMajor ), static_cast <unsigned> ( TempMinor ) );
        }
    else
        {
        // Still don't have the version. Try to get it with GL_VERSION...
        const GLubyte *GLVersionString = glGetString ( GL_VERSION );
        if ( GLVersionString == nullptr )
            return false;
        // < v3.0; resort to the old-fashioned way.
        LOG_DEBUG ( "Reported GL version string : '%s'", GLVersionString );
        if ( OpenGLVersion.ParseString ( reinterpret_cast <const char *> ( GLVersionString ) ) == false )
            return false;
        }
    LOG_DEBUG ( "OpenGL version %d.%d", OpenGLVersion.Major, OpenGLVersion.Minor );

    /**
    OpenGL 	#version 	latest spec version number 	Note
    2.0 	100 	1.10.59
    2.1 	120 	1.20.8
    3.0 	130 	1.30.10
    3.1 	140 	1.40.08
    3.2 	150 	1.50.11 	added Geometry Shaders
    3.3 	330 	3.30.6
    4.0 	400 	4.00.9 	added both Tessellation Shaders
    4.1 	410 	4.10.6
    4.2 	420 	4.20.11
    4.3 	430 	4.30.6 	 added Compute Shaders
    */

    // Safe assumptions about GLSL
    if ( OpenGLVersion.GreaterEqual ( 2, 0 ) ) GLSLVersion.Set ( 1, 0 );
    if ( OpenGLVersion.GreaterEqual ( 2, 1 ) ) GLSLVersion.Set ( 1, 2 );
    if ( OpenGLVersion.GreaterEqual ( 3, 0 ) ) GLSLVersion.Set ( 1, 3 );
    if ( OpenGLVersion.GreaterEqual ( 3, 1 ) ) GLSLVersion.Set ( 1, 4 );
    if ( OpenGLVersion.GreaterEqual ( 3, 2 ) ) GLSLVersion.Set ( 1, 5 );
    if ( OpenGLVersion.GreaterEqual ( 3, 3 ) ) GLSLVersion = OpenGLVersion;

    const GLubyte *GLSLVersionString = glGetString ( GL_SHADING_LANGUAGE_VERSION );
    if ( GLSLVersionString != nullptr )
        {
        LOG_DEBUG ( "Reported GLSL version string : '%s'", GLSLVersionString );
        if ( GLSLVersion.ParseString ( reinterpret_cast <const char *> ( GLSLVersionString ) ) == false )
            LOG_ERROR ( "Error parsing GLSL version string. Assuming safe values" );
        }
    LOG_DEBUG ( "GLSL version %d.%d", GLSLVersion.Major, GLSLVersion.Minor );

    const char *String = reinterpret_cast<const char *> ( glGetString ( GL_RENDERER ) );
    if ( ( String != nullptr ) && ( strlen ( String ) > 0 ) )
        OpenGLRendererString.assign ( String );

    LOG_DEBUG ( "OpenGL renderer string : '%s'", OpenGLRendererString.c_str() );
    for ( GLenum iterator = 0; iterator < /*std::numeric_limits<unsigned>::max ()*/3; ++iterator )
        {
        String = reinterpret_cast<const char *> ( glGetString ( GL_VENDOR + iterator ) );
        if ( ( String != nullptr ) && ( strlen ( String ) > 0 ) )
            {
            if ( OpenGLVendorString.length () > 0 )
                OpenGLVendorString.append ( "\n" );
            OpenGLVendorString.append ( String );
            }
        else
            break;
        }
    LOG_DEBUG ( "OpenGL vendor string : '%s'", OpenGLVendorString.c_str () );

    Extensions.clear ();
    // Method 1 - glGetStringi is only guaranteed to be available after glcore/gles 3.0
    if ( OpenGLVersion.GreaterEqual ( 3, 0 ) )
        {
        // Try to get extensions using the index
        int ExtensionCount = 0;
        glGetIntegerv ( GL_NUM_EXTENSIONS, &ExtensionCount );
        if ( ( glGetError () == GL_NO_ERROR ) && ( ExtensionCount != 0 ) && ( glGetStringi != nullptr ) )
            {
            Extensions.reserve ( ExtensionCount );
            for ( int ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ++ExtensionIndex )
                {
                const char *CurrentExtension = ( const char * ) glGetStringi ( GL_EXTENSIONS, ExtensionIndex );
                if ( CurrentExtension == nullptr ) continue;
                Extensions.push_back ( CurrentExtension );
                }
            }
        }
    // Method 2
    // Profiles >= 3.0 are not guaranteed to have GL_EXTENSIONS support for glGetString
    if ( Extensions.empty () )
        {
        const char *ExtensionString, *Begin, *End;
        ExtensionString = ( const char * ) glGetString ( GL_EXTENSIONS );
        if ( ExtensionString == NULL )
            return false;
        if ( glGetError () == GL_NO_ERROR )
            {
            unsigned Length = ( unsigned ) strlen ( ExtensionString );
            for ( Begin = ExtensionString; Begin < ExtensionString + Length; Begin = End + 1 )
                {
                End = strchr ( Begin, ' ' );
                if ( End == NULL ) End = ExtensionString + Length;

                std::string NewExtension;
                NewExtension.assign ( Begin, End );
                Extensions.push_back ( NewExtension );
                }
            }
        }
    std::sort ( Extensions.begin (), Extensions.end () );
#if 1
    LOG_DEBUG ( "Detected extensions: %lu", ( unsigned long ) Extensions.size () );
    for ( auto Iterator : Extensions )
        LOG_DEBUG ( "%s", Iterator.c_str () );
#endif
    return true;
    }
}
}
