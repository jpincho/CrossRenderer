#pragma once
#include "WindowManager/WindowManagerDefinitions.h"

namespace CrossRenderer
{
enum class RendererBackend
    {
    OpenGLCore,
    };

typedef struct RendererConfiguration
    {
    RendererConfiguration ( void )
        {
        VSyncEnabled = true;
        RedBits = GreenBits = BlueBits = AlphaBits = 8;
        DepthBits = 24;
        StencilBits = 8;
        DesiredRendererBackend = ( RendererBackend ) 0;
        DesiredWindowBackend = ( WindowManagerBackend ) 0;
        }
    std::vector <RenderWindowDescriptor> InitialWindowConfigurations;
    WindowManagerBackend DesiredWindowBackend;
    RendererBackend DesiredRendererBackend;
    bool VSyncEnabled;
    uint8_t RedBits, GreenBits, BlueBits, AlphaBits, DepthBits, StencilBits;
    } RendererConfiguration;

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

typedef struct BlendSettings
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
    bool operator == ( const BlendSettings &other ) const
        {
        return ( ( Enabled == other.Enabled ) && ( Source == other.Source ) && ( Destination == other.Destination ) );
        }
    bool Enabled;
    BlendMode Source, Destination;
    } BlendSettings;

typedef struct StencilBufferSettings
    {
    StencilBufferSettings ( void )
        {
        Enabled = false;
        Mask = std::numeric_limits <unsigned>::max ();
        Function = StencilFunction::Always;
        FunctionReference = FunctionMask = std::numeric_limits <unsigned>::max ();
        OnFail = OnFailZ = OnPassZ = StencilFailAction::Keep;
        }
    bool operator == ( const StencilBufferSettings &other ) const
        {
        return ( ( Enabled == other.Enabled ) && ( Mask == other.Mask ) && ( Function == other.Function ) && ( FunctionReference == other.FunctionReference ) && ( FunctionMask == other.FunctionMask ) &&
                 ( OnFail == other.OnFail ) && ( OnFailZ == other.OnFailZ ) && ( OnPassZ == other.OnPassZ ) );
        }
    bool Enabled;
    unsigned Mask;
    StencilFunction Function;
    unsigned FunctionReference, FunctionMask;
    StencilFailAction OnFail, OnFailZ, OnPassZ;
    } StencilBufferSettings;

typedef struct ScissorSettings
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
    bool operator == ( const ScissorSettings &other ) const
        {
        return ( ( Enabled == other.Enabled ) && ( LowerLeft == other.LowerLeft ) && ( Dimensions == other.Dimensions ) );
        }
    bool Enabled;
    glm::uvec2 LowerLeft, Dimensions;
    } ScissorSettings;

typedef struct ViewportSettings
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
    bool operator == ( const ViewportSettings &other ) const
        {
        return ( ( Enabled == other.Enabled ) && ( LowerLeft == other.LowerLeft ) && ( Dimensions == other.Dimensions ) );
        }
    bool Enabled;
    glm::uvec2 LowerLeft, Dimensions;
    } ViewportSettings;

typedef struct CullingSettings
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
    bool operator == ( const CullingSettings &other ) const
        {
        return ( ( Enabled == other.Enabled ) && ( Mode == other.Mode ) && ( Winding == other.Winding ) );
        }
    bool Enabled;
    CullingMode Mode;
    CullingFaceWinding Winding;
    } CullingSettings;

typedef struct DepthTestSettings
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
    bool operator == ( const DepthTestSettings &other ) const
        {
        return ( ( Enabled == other.Enabled ) && ( Mode == other.Mode ) );
        }
    bool Enabled;
    DepthTestMode Mode;
    } DepthTestSettings;

typedef struct RenderState
    {
    BlendSettings Blending;
    CullingSettings Culling;
    StencilBufferSettings Stencil;
    ScissorSettings Scissor;
    ViewportSettings Viewport;
    DepthTestSettings DepthTest;
    } RenderState;
}
