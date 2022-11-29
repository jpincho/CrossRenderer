#pragma once
#include "../RendererDefinitions.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
class StateCache
    {
    protected:
        RenderState CurrentState;
        glm::uvec2 DefaultViewportSize;

    public:
        bool Enabled = true;
        void SetDefaultViewportSize ( const glm::uvec2 NewSize );
        void ApplyState ( const RenderState &NewState );
        void Invalidate ( void );
        RenderState GetCurrentState ( void );
        void ConfigureCulling ( const CullingSettings &NewState );
        void ConfigureBlending ( const BlendSettings &NewSettings );
        void ConfigureStencil ( const StencilBufferSettings &NewSettings );
        void ConfigureScissor ( const ScissorSettings &NewSettings );
        void ConfigureViewport ( const ViewportSettings &NewSettings );
        void ConfigureDepthTest ( const DepthTestSettings &NewSettings );
    };
}
}
