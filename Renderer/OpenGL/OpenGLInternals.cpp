#include "OpenGLInternals.hpp"
#include <stdexcept>
#include "../Internal/Logger.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
RenderState CurrentState;
FramebufferHandle CurrentBoundFramebuffer;

VectorizedContainer <ShaderBufferInfo> ShaderBuffers;
VectorizedContainer <TextureInfo> Textures;
VectorizedContainer <ShaderInfo> Shaders;
VectorizedContainer <ShaderObjectInfo> ShaderObjects;
VectorizedContainer <FramebufferInfo> Framebuffers;
OpenGLInformationStruct OpenGLInformation;

void OpenGLMessageCallback ( GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei, GLchar const *Message, void const * )
	{
	if ( ( Type == GL_DEBUG_TYPE_OTHER ) && ( Severity == GL_DEBUG_SEVERITY_NOTIFICATION ) )
		return;
	if ( Type == GL_DEBUG_TYPE_ERROR )
		LOG_ERROR ( "%s - %s - %s - %X - %s", StringifyOpenGL ( Source ), StringifyOpenGL ( Type ), StringifyOpenGL ( Severity ), ID, Message );
	else
		LOG_DEBUG ( "%s - %s - %s - %X - %s", StringifyOpenGL ( Source ), StringifyOpenGL ( Type ), StringifyOpenGL ( Severity ), ID, Message );
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

			STRINGIFY ( GL_DEBUG_SOURCE_API );
			STRINGIFY ( GL_DEBUG_SOURCE_WINDOW_SYSTEM );
			STRINGIFY ( GL_DEBUG_SOURCE_SHADER_COMPILER );
			STRINGIFY ( GL_DEBUG_SOURCE_THIRD_PARTY );
			STRINGIFY ( GL_DEBUG_SOURCE_APPLICATION );
			STRINGIFY ( GL_DEBUG_SOURCE_OTHER );

			STRINGIFY ( GL_DEBUG_TYPE_ERROR );
			STRINGIFY ( GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR );
			STRINGIFY ( GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR );
			STRINGIFY ( GL_DEBUG_TYPE_PORTABILITY );
			STRINGIFY ( GL_DEBUG_TYPE_PERFORMANCE );
			STRINGIFY ( GL_DEBUG_TYPE_MARKER );
			STRINGIFY ( GL_DEBUG_TYPE_OTHER );

			STRINGIFY ( GL_DEBUG_SEVERITY_NOTIFICATION );
			STRINGIFY ( GL_DEBUG_SEVERITY_LOW );
			STRINGIFY ( GL_DEBUG_SEVERITY_MEDIUM );
			STRINGIFY ( GL_DEBUG_SEVERITY_HIGH );

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
		Error = glGetError ();
		if ( Error != GL_NO_ERROR )
			{
			Result = false;
			std::string ErrorString ( "OpenGL error - " );
			ErrorString.append ( StringifyOpenGL ( Error ) );
			LOG_ERROR ( ErrorString.c_str () );
			throw std::runtime_error ( ErrorString.c_str () );
			}
		}
	while ( Error != GL_NO_ERROR );
	return Result;
#endif
	}

GLenum Translate ( const DepthTestMode Value )
	{
	static GLenum Values[] =
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
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const BlendMode Value )
	{
	static GLenum Values[] =
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
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const StencilFunction Value )
	{
	static GLenum Values[] =
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
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const StencilFailAction Value )
	{
	static GLenum Values[] =
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
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const CullingMode Value )
	{
	static GLenum Values[] =
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
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const CullingFaceWinding Value )
	{
	static GLenum Values[] =
		{
		GL_CW,
		GL_CCW,
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid winding mode" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const ShaderBufferComponentType Value )
	{
	static GLenum Values[] =
		{
		GL_FLOAT,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_SHORT,
		GL_UNSIGNED_INT,
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid component type" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const ShaderBufferAccessType Value )
	{
	static GLenum Values[] =
		{
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_STREAM_DRAW
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid access type" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const ShaderBufferMapAccessType Value )
	{
	static GLenum Values[] =
		{
		GL_READ_ONLY,
		GL_WRITE_ONLY,
		GL_READ_WRITE
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid map access type" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const ShaderBufferType Value )
	{
	static GLenum Values[] =
		{
		GL_ARRAY_BUFFER,
		GL_UNIFORM_BUFFER,
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid buffer type" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const PrimitiveType Value )
	{
	static GLenum Values[] =
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
	return Values[static_cast <unsigned> ( Value )];
	}

GLint Translate ( const TextureFilter Value )
	{
	static GLint Values[] =
		{
		GL_LINEAR,
		GL_NEAREST,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid texture filter" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLint Translate ( const TextureWrapMode Value )
	{
	static GLint Values[] =
		{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid texture wrap mode" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

GLenum Translate ( const PolygonMode Value )
	{
	static GLenum Values[] =
		{
		GL_POINT,
		GL_LINE,
		GL_FILL
		};
	static unsigned Count = sizeof ( Values ) / sizeof ( GLenum );
	if ( static_cast <unsigned> ( Value ) >= Count )
		throw std::runtime_error ( std::string ( "Invalid polygon mode" ) );
	return Values[static_cast <unsigned> ( Value )];
	}

ShaderUniformType TranslateOpenGLUniformType ( GLenum Type )
	{
	switch ( Type )
		{
		case GL_FLOAT:
			return ShaderUniformType::Float;
		case GL_FLOAT_VEC2:
			return ShaderUniformType::Float2;
		case GL_FLOAT_VEC3:
			return ShaderUniformType::Float3;
		case GL_FLOAT_VEC4:
			return ShaderUniformType::Float4;

		case GL_INT:
			return ShaderUniformType::Integer;
		case GL_INT_VEC2:
			return ShaderUniformType::Integer2;
		case GL_INT_VEC3:
			return ShaderUniformType::Integer3;
		case GL_INT_VEC4:
			return ShaderUniformType::Integer4;

		case GL_UNSIGNED_INT:
			return ShaderUniformType::UnsignedInteger;
#if defined GL_UNSIGNED_INT_VEC2
		case GL_UNSIGNED_INT_VEC2:
			return ShaderUniformType::UnsignedInteger2;
		case GL_UNSIGNED_INT_VEC3:
			return ShaderUniformType::UnsignedInteger3;
		case GL_UNSIGNED_INT_VEC4:
			return ShaderUniformType::UnsignedInteger4;
#endif
		case GL_BOOL:
			return ShaderUniformType::Bool;
		case GL_BOOL_VEC2:
			return ShaderUniformType::Bool2;
		case GL_BOOL_VEC3:
			return ShaderUniformType::Bool3;
		case GL_BOOL_VEC4:
			return ShaderUniformType::Bool4;

		case GL_FLOAT_MAT2:
			return ShaderUniformType::Matrix2;
		case GL_FLOAT_MAT3:
			return ShaderUniformType::Matrix3;
		case GL_FLOAT_MAT4:
			return ShaderUniformType::Matrix4;
		case GL_SAMPLER_2D:
			return ShaderUniformType::Sampler2D;
		case GL_SAMPLER_3D:
			return ShaderUniformType::Sampler3D;
		case GL_SAMPLER_CUBE:
			return ShaderUniformType::SamplerCube;
		default:
			throw std::runtime_error ( "Unhandled shader uniform type" );
		}
	}

GLenum TranslateUniformType ( ShaderUniformType Type )
	{
	switch ( Type )
		{
		case ShaderUniformType::Float:
			return GL_FLOAT;
		case ShaderUniformType::Float2:
			return GL_FLOAT_VEC2;
		case ShaderUniformType::Float3:
			return GL_FLOAT_VEC3;
		case ShaderUniformType::Float4:
			return GL_FLOAT_VEC4;

		case ShaderUniformType::Integer:
			return GL_INT;
		case ShaderUniformType::Integer2:
			return GL_INT_VEC2;
		case ShaderUniformType::Integer3:
			return GL_INT_VEC3;
		case ShaderUniformType::Integer4:
			return GL_INT_VEC4;

		case ShaderUniformType::UnsignedInteger:
			return GL_UNSIGNED_INT;
#if defined GL_UNSIGNED_INT_VEC2
		case ShaderUniformType::UnsignedInteger2:
			return GL_UNSIGNED_INT_VEC2;
		case ShaderUniformType::UnsignedInteger3:
			return GL_UNSIGNED_INT_VEC3;
		case ShaderUniformType::UnsignedInteger4:
			return GL_UNSIGNED_INT_VEC4;
#endif

		case ShaderUniformType::Bool:
			return GL_BOOL;
		case ShaderUniformType::Bool2:
			return GL_BOOL_VEC2;
		case ShaderUniformType::Bool3:
			return GL_BOOL_VEC3;
		case ShaderUniformType::Bool4:
			return GL_BOOL_VEC4;

		case ShaderUniformType::Matrix2:
			return GL_FLOAT_MAT2;
		case ShaderUniformType::Matrix3:
			return GL_FLOAT_MAT3;
		case ShaderUniformType::Matrix4:
			return GL_FLOAT_MAT4;

		case ShaderUniformType::Sampler2D:
			return GL_SAMPLER_2D;
		case ShaderUniformType::Sampler3D:
			return GL_SAMPLER_3D;
		case ShaderUniformType::SamplerCube:
			return GL_SAMPLER_CUBE;
		default:
			throw "Unhandled shader uniform type";
		}
	}
}
}
