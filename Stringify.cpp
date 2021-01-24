#include "Stringify.h"
#include <stdexcept>

namespace CrossRenderer
{
const char *Stringify ( const RendererBackend Value )
    {
#define STRINGIFY(X) case RendererBackend::X: return #X;
    switch ( Value )
        {
            STRINGIFY ( OpenGLCore );
        }
#undef STRINGIFY
    throw std::runtime_error ( "Unhandled graphics API" );
    }

const char *Stringify ( const WindowEventType Value )
    {
#define STRINGIFY(X) case WindowEventType::X: return #X;
    switch ( Value )
        {
            STRINGIFY ( WindowCreated );
            STRINGIFY ( WindowClosed );
            STRINGIFY ( WindowMoved );
            STRINGIFY ( WindowResized );
            STRINGIFY ( KeyPressed );
            STRINGIFY ( KeyReleased );
            STRINGIFY ( ButtonPressed );
            STRINGIFY ( ButtonReleased );
            STRINGIFY ( MouseWheel );
            STRINGIFY ( MouseMoved );
            STRINGIFY ( Quit );
            STRINGIFY ( FrameStarted );
            STRINGIFY ( FrameEnded );
            STRINGIFY ( TextInput );
        }
#undef STRINGIFY
    throw std::runtime_error ( "Unhandled window event type" );
    }

const char *Stringify ( const WindowManagerBackend Value )
    {
#define STRINGIFY(X) case WindowManagerBackend::X: return #X;
    switch ( Value )
        {
            STRINGIFY ( SDL2 );
            STRINGIFY ( Windows );
        }
#undef STRINGIFY
    throw std::runtime_error ( "Unhandled window manager backend type" );
    }

const char* Stringify ( const WindowState Value )
    {
#define STRINGIFY(X) case WindowState::X: return #X;
    switch ( Value )
        {
            STRINGIFY ( Minimized );
            STRINGIFY ( Normal );
            STRINGIFY ( Maximized );
        }
#undef STRINGIFY
    throw std::runtime_error ( "Unhandled window staete" );
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

const char *Stringify ( const ShaderUniformType Type )
    {
#define STRINGIFY(X) case ShaderUniformType::X: return #X;
    switch ( Type )
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
const char *Stringify ( const PixelFormat Format )
    {
#define STRINGIFY(X) case PixelFormat::X: return #X;
    switch ( Format )
        {
            STRINGIFY ( DepthComponent );
            STRINGIFY ( RedGreenBlue888 );
            STRINGIFY ( RedGreenBlueAlpha8888 );
        }
#undef STRINGIFY
    throw std::runtime_error ( "Unhandled pixel format" );
    }
}
