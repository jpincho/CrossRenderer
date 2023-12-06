#pragma once
#include <vector>
#include "ShaderBufferDefinitions.hpp"
#include "ShaderDefinitions.hpp"
#include "TextureDefinitions.hpp"
#include "FramebufferDefinitions.hpp"
#include "RendererDefinitions.hpp"

namespace CrossRenderer
{
enum class PrimitiveType : uint8_t
	{
	Points = 0,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip
	};

enum class BlendMode : uint8_t
	{
	Zero = 0,
	One,
	SourceColor,
	OneMinusSourceColor,
	SourceAlpha,
	OneMinusSourceAlpha,
	DestinationAlpha,
	OneMinusDestinationAlpha,
	DestinationColor,
	OneMinusDestinationColor,
	SourceAlphaSaturate,
	BlendModeLast
	};

enum class StencilFunction : uint8_t
	{
	Never = 0,
	Less,
	LessOrEqual,
	Greater,
	GreaterOrEqual,
	Equal,
	NotEqual,
	Always
	};

enum class StencilFailAction : uint8_t
	{
	Zero = 0,
	Keep,
	Replace,
	Increment,
	Decrement,
	Invert
	};

enum class CullingMode : uint8_t
	{
	Front = 0,
	Back,
	FrontAndBack
	};

enum class CullingFaceWinding : uint8_t
	{
	Clockwise = 0,
	CounterClockwise
	};

enum class DepthTestMode : uint8_t
	{
	Never = 0,
	Less,
	LessOrEqual,
	Greater,
	GreaterOrEqual,
	Equal,
	NotEqual,
	Always
	};

enum class PolygonMode: uint8_t
	{
	Point = 0,
	Line,
	Fill
	};

struct BlendSettings
	{
	BlendSettings ( void )
		{
		Source = BlendMode::SourceAlpha;
		Destination = BlendMode::OneMinusSourceAlpha;
		Enabled = true;
		}
	BlendSettings ( BlendMode NewSource, BlendMode NewDestination )
		{
		Set ( NewSource, NewDestination );
		}
	void Set ( const BlendMode NewSource, const BlendMode NewDestination )
		{
		Enabled = true;
		Source = NewSource;
		Destination = NewDestination;
		}
	bool operator == ( const BlendSettings &Other ) const
		{
		return ( ( Enabled == Other.Enabled ) && ( Source == Other.Source ) && ( Destination == Other.Destination ) );
		}
	bool Enabled;
	BlendMode Source, Destination;
	};

struct StencilBufferSettings
	{
	StencilBufferSettings ( void )
		{
		Enabled = false;
		Mask = std::numeric_limits <unsigned>::max();
		Function = StencilFunction::Always;
		FunctionReference = FunctionMask = std::numeric_limits <unsigned>::max();
		OnFail = OnFailZ = OnPassZ = StencilFailAction::Keep;
		}
	bool operator == ( const StencilBufferSettings &Other ) const
		{
		return ( ( Enabled == Other.Enabled ) && ( Mask == Other.Mask ) && ( Function == Other.Function ) && ( FunctionReference == Other.FunctionReference ) && ( FunctionMask == Other.FunctionMask ) &&
		         ( OnFail == Other.OnFail ) && ( OnFailZ == Other.OnFailZ ) && ( OnPassZ == Other.OnPassZ ) );
		}
	bool Enabled;
	unsigned Mask;
	StencilFunction Function;
	unsigned FunctionReference, FunctionMask;
	StencilFailAction OnFail, OnFailZ, OnPassZ;
	};

struct ScissorSettings
	{
	ScissorSettings ( void )
		{
		Enabled = false;
		}
	void Set ( const glm::uvec2 NewLowerLeft, const glm::uvec2 NewDimensions )
		{
		Enabled = true;
		LowerLeft = NewLowerLeft;
		Dimensions = NewDimensions;
		}
	bool operator == ( const ScissorSettings &Other ) const
		{
		return ( ( Enabled == Other.Enabled ) && ( LowerLeft == Other.LowerLeft ) && ( Dimensions == Other.Dimensions ) );
		}
	bool Enabled;
	glm::uvec2 LowerLeft, Dimensions;
	};

struct ViewportSettings
	{
	ViewportSettings ( void )
		{
		Enabled = false;
		}
	void Set ( const glm::uvec2 NewLowerLeft, const glm::uvec2 NewDimensions )
		{
		Enabled = true;
		LowerLeft = NewLowerLeft;
		Dimensions = NewDimensions;
		}
	bool operator == ( const ViewportSettings &Other ) const
		{
		return ( ( Enabled == Other.Enabled ) && ( LowerLeft == Other.LowerLeft ) && ( Dimensions == Other.Dimensions ) );
		}
	bool Enabled;
	glm::uvec2 LowerLeft, Dimensions;
	};

struct CullingSettings
	{
	CullingSettings ( void )
		{
		Enabled = false;
		Mode = CullingMode::Back;
		Winding = CullingFaceWinding::CounterClockwise;
		}
	void Set ( const CullingMode NewMode, const CullingFaceWinding NewWinding = CullingFaceWinding::CounterClockwise )
		{
		Enabled = true;
		Mode = NewMode;
		Winding = NewWinding;
		}
	bool operator == ( const CullingSettings &Other ) const
		{
		return ( ( Enabled == Other.Enabled ) && ( Mode == Other.Mode ) && ( Winding == Other.Winding ) );
		}
	bool Enabled;
	CullingMode Mode;
	CullingFaceWinding Winding;
	};

struct DepthTestSettings
	{
	DepthTestSettings ( void )
		{
		Enabled = false;
		Mode = DepthTestMode::LessOrEqual;
		}
	void Set ( const DepthTestMode NewMode )
		{
		Enabled = true;
		Mode = NewMode;
		}
	bool operator == ( const DepthTestSettings &Other ) const
		{
		return ( ( Enabled == Other.Enabled ) && ( Mode == Other.Mode ) );
		}
	bool Enabled;
	DepthTestMode Mode;
	};

struct FramebufferSettings
	{
	bool operator == ( const FramebufferSettings &Other ) const
		{
		return ( TargetFramebuffer == Other.TargetFramebuffer );
		}
	FramebufferHandle TargetFramebuffer;
	};

struct PolygonModeSettings
	{
	PolygonModeSettings ( void )
		{
		State = PolygonMode::Fill;
		}
	void Set ( const PolygonMode NewState )
		{
		State = NewState;
		}
	bool operator == ( const PolygonModeSettings &Other ) const
		{
		return ( State == Other.State );
		}
	PolygonMode State;
	};

struct RenderState
	{
	BlendSettings Blending;
	CullingSettings Culling;
	StencilBufferSettings Stencil;
	ScissorSettings Scissor;
	ViewportSettings Viewport;
	DepthTestSettings DepthTest;
	PolygonModeSettings PolygonMode;
	};

struct ShaderTextureBindPair
	{
	ShaderUniformHandle UniformHandle;
	TextureBindSettings BindSettings;
	ShaderTextureBindPair ( const ShaderUniformHandle &NewUniformHandle, const TextureBindSettings NewBindSettings ) : UniformHandle ( NewUniformHandle ), BindSettings ( NewBindSettings )
		{}
	};

struct ShaderBufferBindPair
	{
	ShaderBufferBindPair ( const ShaderAttributeHandle &NewAttributeHandle, const ShaderBufferDataStream &NewDataStream ) : AttributeHandle ( NewAttributeHandle ), DataStream ( NewDataStream )
		{}
	ShaderAttributeHandle AttributeHandle;
	ShaderBufferDataStream DataStream;
	};

struct ShaderUniformValuePair
	{
	ShaderUniformValuePair ( const ShaderUniformHandle &NewUniformHandle, const ShaderUniformValue &NewUniformValue ) : UniformHandle ( NewUniformHandle ), UniformValue ( NewUniformValue )
		{}
	ShaderUniformHandle UniformHandle;
	ShaderUniformValue UniformValue;
	};

struct RenderCommand
	{
	std::vector <ShaderBufferBindPair> ShaderBufferBindings;
	std::vector <ShaderTextureBindPair> TextureBindings;
	std::vector <ShaderUniformValuePair> UniformValues;
	ShaderHandle Shader;
	ShaderBufferDataStream IndexBufferStream;
	PrimitiveType Primitive;
	size_t VertexCount, StartVertex;

	size_t InstanceCount;
	RenderState State;
	};
}

