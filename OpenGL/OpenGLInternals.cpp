#include "OpenGLInternals.h"
#include <Logger/Logger.h>
#include <stdexcept>

namespace CrossRenderer
{
namespace OpenGL
{
VectorizedContainer <ShaderBufferInfo, ShaderBufferTag> ShaderBuffers;
VectorizedContainer <TextureInfo, TextureTag> Textures;
VectorizedContainer <ShaderInfo, ShaderTag> Shaders;
VectorizedContainer <FramebufferInfo, FramebufferTag> Framebuffers;

void OpenGLMessageCallback ( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, GLchar const* message, void const*  )
    {
#define STRINGIFY(X) case GL_DEBUG_##X: return #X;
    auto const src_str = [source]()
        {
        switch ( source )
            {
                STRINGIFY ( SOURCE_API );
                STRINGIFY ( SOURCE_WINDOW_SYSTEM );
                STRINGIFY ( SOURCE_SHADER_COMPILER );
                STRINGIFY ( SOURCE_THIRD_PARTY );
                STRINGIFY ( SOURCE_APPLICATION );
                STRINGIFY ( SOURCE_OTHER );
            default:
                return "UNKNOWN SOURCE";
            }
        }
    ();

    auto const type_str = [type]()
        {
        switch ( type )
            {
                STRINGIFY ( TYPE_ERROR );
                STRINGIFY ( TYPE_DEPRECATED_BEHAVIOR );
                STRINGIFY ( TYPE_UNDEFINED_BEHAVIOR );
                STRINGIFY ( TYPE_PORTABILITY );
                STRINGIFY ( TYPE_PERFORMANCE );
                STRINGIFY ( TYPE_MARKER );
                STRINGIFY ( TYPE_OTHER );
            default:
                return "UNKNOWN TYPE";
            }
        }
    ();

    auto const severity_str = [severity]()
        {
        switch ( severity )
            {
                STRINGIFY ( SEVERITY_NOTIFICATION );
                STRINGIFY ( SEVERITY_LOW );
                STRINGIFY ( SEVERITY_MEDIUM );
                STRINGIFY ( SEVERITY_HIGH );
            default:
                return "UNKNOWN SEVERITY";
            }
        }
    ();
#undef STRINGIFY
    if ( ( type == GL_DEBUG_TYPE_OTHER ) && ( severity == GL_DEBUG_SEVERITY_NOTIFICATION ) )
        return;
    if ( type == GL_DEBUG_TYPE_ERROR )
        LOG_ERROR ( "%s - %s - %s - %X - %s", src_str, type_str, severity_str, id, message );
    else
        LOG_DEBUG ( "%s - %s - %s - %X - %s", src_str, type_str, severity_str, id, message );
    }

const char *StringifyOpenGL ( GLenum Value )
    {
    switch ( Value )
        {
#define STRINGIFY(X) case X: return #X;
            STRINGIFY ( GL_FRAMEBUFFER_UNDEFINED );
            STRINGIFY ( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT );
            STRINGIFY ( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT );
            STRINGIFY ( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER );
            STRINGIFY ( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER );
            STRINGIFY ( GL_FRAMEBUFFER_UNSUPPORTED );
            STRINGIFY ( GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE );
            STRINGIFY ( GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS );
            STRINGIFY ( GL_FRAMEBUFFER_COMPLETE );

            STRINGIFY ( GL_NO_ERROR );
            STRINGIFY ( GL_INVALID_ENUM );
            STRINGIFY ( GL_INVALID_VALUE );
            STRINGIFY ( GL_INVALID_OPERATION );
            STRINGIFY ( GL_STACK_OVERFLOW );
            STRINGIFY ( GL_STACK_UNDERFLOW );
            STRINGIFY ( GL_OUT_OF_MEMORY );
            STRINGIFY ( GL_TABLE_TOO_LARGE );
        default:
            throw std::runtime_error ( "Unhandled OpenGL enum" );
        }
#undef STRINGIFY
    }

bool CheckError ( void )
    {
#if defined ( CROSS_RENDERER_OPENGL_DISABLE_CHECK_ERROR )
    return true;
#else
    GLenum Error;
    bool Result = true;
    do
        {
        Error = glGetError();
        if ( Error != GL_NO_ERROR )
            {
            Result = false;
            throw std::runtime_error ( std::string ( "OpenGL error" ) + std::string ( StringifyOpenGL ( Error ) ) );
            }
        }
    while ( Error != GL_NO_ERROR );
    return Result;
#endif
    }

GLenum Translate ( const DepthTestMode Value )
    {
    static GLenum Values [] =
        {
        GL_NEVER,
        GL_LESS,
        GL_LEQUAL,
        GL_GREATER,
        GL_GEQUAL,
        GL_EQUAL,
        GL_NOTEQUAL,
        GL_ALWAYS
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid depth function" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const BlendMode Value )
    {
    static GLenum Values [] =
        {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA_SATURATE
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid blend mode" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const StencilFunction Value )
    {
    static GLenum Values [] =
        {
        GL_NEVER,
        GL_LESS,
        GL_LEQUAL,
        GL_GREATER,
        GL_GEQUAL,
        GL_EQUAL,
        GL_NOTEQUAL,
        GL_ALWAYS
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid stencil function" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const StencilFailAction Value )
    {
    static GLenum Values [] =
        {
        GL_ZERO,
        GL_KEEP,
        GL_REPLACE,
        GL_INCR,
        GL_DECR,
        GL_INVERT
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid stencil fail action" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const CullingMode Value )
    {
    static GLenum Values [] =
        {
        GL_FRONT,
        GL_BACK,
        GL_FRONT_AND_BACK,
        GL_INCR,
        GL_DECR,
        GL_INVERT
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid culling mode" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const CullingFaceWinding Value )
    {
    static GLenum Values [] =
        {
        GL_CW,
        GL_CCW,
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid winding mode" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const ShaderBufferComponentType Value )
    {
    static GLenum Values [] =
        {
        GL_FLOAT,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_INT,
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid component type" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const ShaderBufferAccessType Value )
    {
    static GLenum Values [] =
        {
        GL_STATIC_DRAW,
        GL_DYNAMIC_DRAW,
        GL_STREAM_DRAW
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid access type" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const ShaderBufferType Value )
    {
    static GLenum Values [] =
        {
        GL_ARRAY_BUFFER,
        GL_UNIFORM_BUFFER,
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid buffer type" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLenum Translate ( const PrimitiveType Value )
    {
    static GLenum Values [] =
        {
        GL_POINTS,
        GL_LINES,
        GL_LINE_STRIP,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid primitive type" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLint Translate ( const TextureFilter Value )
    {
    static GLint Values [] =
        {
        GL_LINEAR,
        GL_NEAREST,
        GL_NEAREST_MIPMAP_NEAREST,
        GL_NEAREST_MIPMAP_LINEAR,
        GL_LINEAR_MIPMAP_LINEAR
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid texture filter" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }

GLint Translate ( const TextureWrapMode Value )
    {
    static GLint Values [] =
        {
        GL_REPEAT,
        GL_MIRRORED_REPEAT,
        GL_CLAMP_TO_EDGE
        };
    static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
    if ( static_cast <unsigned> ( Value ) >= Count )
        throw std::runtime_error ( std::string ( "Invalid texture wrap mode" ) );
    return Values[ static_cast <unsigned> ( Value )];
    }
}
}
