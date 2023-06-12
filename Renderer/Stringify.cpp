#include "Stringify.hpp"
#include <stdexcept>

namespace CrossRenderer
{
const char *Stringify ( const RendererBackend Value )
	{
#define STRINGIFY(X) case RendererBackend::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( OpenGLCore );
		STRINGIFY ( Direct3D11 );
		default:
			throw std::runtime_error ( "Unhandled graphics API" );
		}
#undef STRINGIFY
	}

const char *Stringify ( const PrimitiveType Value )
	{
#define STRINGIFY(X) case PrimitiveType::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Points );
		STRINGIFY ( LineList );
		STRINGIFY ( LineStrip );
		STRINGIFY ( TriangleList );
		STRINGIFY ( TriangleStrip );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled primitive type" );
	}

const char *Stringify ( const BlendMode Value )
	{
#define STRINGIFY(X) case BlendMode::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Zero );
		STRINGIFY ( One );
		STRINGIFY ( SourceColor );
		STRINGIFY ( OneMinusSourceColor );
		STRINGIFY ( SourceAlpha );
		STRINGIFY ( OneMinusSourceAlpha );
		STRINGIFY ( DestinationAlpha );
		STRINGIFY ( OneMinusDestinationAlpha );
		STRINGIFY ( DestinationColor );
		STRINGIFY ( OneMinusDestinationColor );
		STRINGIFY ( SourceAlphaSaturate );
		STRINGIFY ( BlendModeLast );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled blend mode" );
	}

const char *Stringify ( const StencilFunction Value )
	{
#define STRINGIFY(X) case StencilFunction::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Never );
		STRINGIFY ( Less );
		STRINGIFY ( LessOrEqual );
		STRINGIFY ( Greater );
		STRINGIFY ( GreaterOrEqual );
		STRINGIFY ( Equal );
		STRINGIFY ( NotEqual );
		STRINGIFY ( Always );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled stencil function" );
	}

const char *Stringify ( const StencilFailAction Value )
	{
#define STRINGIFY(X) case StencilFailAction::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Zero );
		STRINGIFY ( Keep );
		STRINGIFY ( Replace );
		STRINGIFY ( Increment );
		STRINGIFY ( Decrement );
		STRINGIFY ( Invert );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled stencil fail action" );
	}

const char *Stringify ( const CullingMode Value )
	{
#define STRINGIFY(X) case CullingMode::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Front );
		STRINGIFY ( Back );
		STRINGIFY ( FrontAndBack );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled culling mode" );
	}

const char *Stringify ( const CullingFaceWinding Value )
	{
#define STRINGIFY(X) case CullingFaceWinding::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Clockwise );
		STRINGIFY ( CounterClockwise );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled culling face winding" );
	}

const char *Stringify ( const DepthTestMode Value )
	{
#define STRINGIFY(X) case DepthTestMode::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Never );
		STRINGIFY ( Less );
		STRINGIFY ( LessOrEqual );
		STRINGIFY ( Greater );
		STRINGIFY ( GreaterOrEqual );
		STRINGIFY ( Equal );
		STRINGIFY ( NotEqual );
		STRINGIFY ( Always );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled depth test mode" );
	}

const char *Stringify ( const ShaderUniformType Value )
	{
#define STRINGIFY(X) case ShaderUniformType::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Bool );
		STRINGIFY ( Bool2 );
		STRINGIFY ( Bool3 );
		STRINGIFY ( Bool4 );
		STRINGIFY ( Float );
		STRINGIFY ( Float2 );
		STRINGIFY ( Float3 );
		STRINGIFY ( Float4 );
		STRINGIFY ( UnsignedInteger );
		STRINGIFY ( UnsignedInteger2 );
		STRINGIFY ( UnsignedInteger3 );
		STRINGIFY ( UnsignedInteger4 );
		STRINGIFY ( Integer );
		STRINGIFY ( Integer2 );
		STRINGIFY ( Integer3 );
		STRINGIFY ( Integer4 );
		STRINGIFY ( Matrix2 );
		STRINGIFY ( Matrix3 );
		STRINGIFY ( Matrix4 );
		STRINGIFY ( Sampler2D );
		STRINGIFY ( Sampler3D );
		STRINGIFY ( SamplerCube );
		STRINGIFY ( Block );
		STRINGIFY ( Unknown );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled shader uniform type" );
	}

const char *Stringify ( const PixelFormat Value )
	{
#define STRINGIFY(X) case PixelFormat::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Red8 );
		STRINGIFY ( RedGreen88 );
		STRINGIFY ( RedGreenBlue888 );
		STRINGIFY ( RedGreenBlueAlpha8888 );
		STRINGIFY ( DepthComponent );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled pixel format" );
	}

const char *Stringify ( const ShaderObjectType Value )
	{
#define STRINGIFY(X) case ShaderObjectType::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Vertex );
		STRINGIFY ( Geometry );
		STRINGIFY ( Fragment );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled shader object type" );
	}

const char *Stringify ( const TextureWrapMode Value )
	{
#define STRINGIFY(X) case TextureWrapMode::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Repeat );
		STRINGIFY ( MirroredRepeat );
		STRINGIFY ( ClampToEdge );
		STRINGIFY ( ClampToBorder );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled texture wrap mode" );
	}

const char *Stringify ( const TextureFilter Value )
	{
#define STRINGIFY(X) case TextureFilter::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Linear );
		STRINGIFY ( Nearest );
		STRINGIFY ( NearestMipmapNearest );
		STRINGIFY ( NearestMipmapLinear );
		STRINGIFY ( LinearMipmapLinear );
		STRINGIFY ( LinearMipmapNearest );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled texture filter" );
	}

const char *Stringify ( const PolygonMode Value )
	{
#define STRINGIFY(X) case PolygonMode::X: return #X;
	switch ( Value )
		{
		STRINGIFY ( Point );
		STRINGIFY ( Line );
		STRINGIFY ( Fill );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled polygon mode" );
	}
}
