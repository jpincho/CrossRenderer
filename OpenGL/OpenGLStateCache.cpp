#include "OpenGLStateCache.h"
#include "OpenGLInternals.h"
#include "../WindowManager/WindowManager.h"

namespace CrossRenderer
{
namespace OpenGL
{
void StateCache::SetDefaultViewportSize ( const glm::uvec2 NewSize )
    {
    DefaultViewportSize = NewSize;
    }

void StateCache::ApplyState ( const RenderState &NewState )
    {
    if ( Enabled )
        {
        ConfigureCulling ( NewState.Culling );
        ConfigureBlending ( NewState.Blending );
        ConfigureStencil ( NewState.Stencil );
        ConfigureScissor ( NewState.Scissor );
        ConfigureViewport ( NewState.Viewport );
        ConfigureDepthTest ( NewState.DepthTest );
        }
    else
        {
        CurrentState = NewState;
        if ( CurrentState.Culling.Enabled )
            glEnable ( GL_CULL_FACE );
        else
            glDisable ( GL_CULL_FACE );
        glCullFace ( Translate ( CurrentState.Culling.Mode ) );
        glFrontFace ( Translate ( CurrentState.Culling.Winding ) );

        if ( CurrentState.Blending.Enabled )
            glEnable ( GL_BLEND );
        else
            glDisable ( GL_BLEND );
        glBlendFunc ( Translate ( CurrentState.Blending.Source ), Translate ( CurrentState.Blending.Destination ) );

        if ( CurrentState.Stencil.Enabled )
            glEnable ( GL_STENCIL_TEST );
        else
            glDisable ( GL_STENCIL_TEST );
        glStencilMask ( CurrentState.Stencil.Mask );
        glStencilFunc ( Translate ( CurrentState.Stencil.Function ), CurrentState.Stencil.FunctionReference, CurrentState.Stencil.FunctionMask );
        glStencilOp ( Translate ( CurrentState.Stencil.OnFail ), Translate ( CurrentState.Stencil.OnFailZ ), Translate ( CurrentState.Stencil.OnPassZ ) );

        if ( CurrentState.Scissor.Enabled )
            glEnable ( GL_SCISSOR_TEST );
        else
            glDisable ( GL_SCISSOR_TEST );
        glScissor ( CurrentState.Scissor.LowerLeft.x, CurrentState.Scissor.LowerLeft.y, CurrentState.Scissor.Dimensions.x, CurrentState.Scissor.Dimensions.y );

        glViewport ( 0, 0, DefaultViewportSize.x, DefaultViewportSize.y );

        if ( CurrentState.DepthTest.Enabled )
            glEnable ( GL_DEPTH_TEST );
        else
            glDisable ( GL_DEPTH_TEST );
        glDepthFunc ( Translate ( CurrentState.DepthTest.Mode ) );
        }
    }

void StateCache::Invalidate ( void )
    {
    CurrentState = RenderState();
//    CurrentState.Viewport = RendererInfo.DefaultViewport;

    if ( CurrentState.Culling.Enabled )
        glEnable ( GL_CULL_FACE );
    else
        glDisable ( GL_CULL_FACE );
    glCullFace ( Translate ( CurrentState.Culling.Mode ) );

    if ( CurrentState.Blending.Enabled )
        glEnable ( GL_BLEND );
    else
        glDisable ( GL_BLEND );
    glBlendFunc ( Translate ( CurrentState.Blending.Source ), Translate ( CurrentState.Blending.Destination ) );

    if ( CurrentState.Stencil.Enabled )
        glEnable ( GL_STENCIL_TEST );
    else
        glDisable ( GL_STENCIL_TEST );
    glStencilMask ( CurrentState.Stencil.Mask );
    glStencilFunc ( Translate ( CurrentState.Stencil.Function ), CurrentState.Stencil.FunctionReference, CurrentState.Stencil.FunctionMask );
    glStencilOp ( Translate ( CurrentState.Stencil.OnFail ), Translate ( CurrentState.Stencil.OnFailZ ), Translate ( CurrentState.Stencil.OnPassZ ) );

    if ( CurrentState.Scissor.Enabled )
        glEnable ( GL_SCISSOR_TEST );
    else
        glDisable ( GL_SCISSOR_TEST );
    glScissor ( CurrentState.Scissor.LowerLeft.x, CurrentState.Scissor.LowerLeft.y, CurrentState.Scissor.Dimensions.x, CurrentState.Scissor.Dimensions.y );

    if ( CurrentState.DepthTest.Enabled )
        glEnable ( GL_DEPTH_TEST );
    else
        glDisable ( GL_DEPTH_TEST );
    glDepthFunc ( Translate ( CurrentState.DepthTest.Mode ) );

    if ( CurrentState.Viewport.Enabled )
        glViewport ( CurrentState.Viewport.LowerLeft.x, CurrentState.Viewport.LowerLeft.y, CurrentState.Viewport.Dimensions.x, CurrentState.Viewport.Dimensions.y );
    else
        ConfigureViewport ( ViewportSettings() );

//    if ( !CurrentState.Framebuffer.targetFramebuffer )
//        glBindFramebuffer ( GL_FRAMEBUFFER, OpenGLFlags.defaultFramebuffer );
//    else
//        bindFramebuffer ( CurrentState.Framebuffer.targetFramebuffer );
    }

RenderState StateCache::GetCurrentState ( void )
    {
    return CurrentState;
    }

void StateCache::ConfigureCulling ( const CullingSettings &NewSettings )
    {
    if ( NewSettings == CurrentState.Culling )
        return;

    if ( NewSettings.Enabled != CurrentState.Culling.Enabled )
        {
        if ( NewSettings.Enabled )
            glEnable ( GL_CULL_FACE );
        else
            glDisable ( GL_CULL_FACE );
        CurrentState.Culling.Enabled = NewSettings.Enabled;
        }
    if ( CurrentState.Culling.Enabled == false )
        return;
    if ( NewSettings.Mode != CurrentState.Culling.Mode )
        glCullFace ( Translate ( NewSettings.Mode ) );
    if ( NewSettings.Winding != CurrentState.Culling.Winding )
        glFrontFace ( Translate ( NewSettings.Winding ) );
    CurrentState.Culling = NewSettings;
    CheckError();
    }

void StateCache::ConfigureBlending ( const BlendSettings &NewSettings )
    {
    if ( NewSettings == CurrentState.Blending )
        return;

    // This is basically disabling blending...
    if ( ( ( NewSettings.Source == BlendMode::One ) && ( NewSettings.Destination == BlendMode::Zero ) ) || ( NewSettings.Enabled == false ) )
        {
        glDisable ( GL_BLEND );
        CurrentState.Blending.Enabled = false;
        return;
        }

    if ( NewSettings.Enabled != CurrentState.Blending.Enabled )
        {
        if ( NewSettings.Enabled )
            glEnable ( GL_BLEND );
        else
            glDisable ( GL_BLEND );
        CurrentState.Blending.Enabled = NewSettings.Enabled;
        }
    if ( CurrentState.Blending.Enabled == false )
        return;
    if ( ( NewSettings.Source != CurrentState.Blending.Source ) || ( NewSettings.Destination != CurrentState.Blending.Destination ) )
        {
        glBlendFunc ( Translate ( NewSettings.Source ), Translate ( NewSettings.Destination ) );
        }

    CurrentState.Blending = NewSettings;
    CheckError();
    }

void StateCache::ConfigureStencil ( const StencilBufferSettings &NewSettings )
    {
    if ( NewSettings == CurrentState.Stencil )
        return;

    // If there's a change in state...
    if ( NewSettings.Enabled != CurrentState.Stencil.Enabled )
        {
        if ( NewSettings.Enabled )
            glEnable ( GL_STENCIL_TEST );
        else
            glDisable ( GL_STENCIL_TEST );
        CurrentState.Stencil.Enabled = NewSettings.Enabled;
        }

    if ( CurrentState.Stencil.Enabled == false )
        return;

    if ( NewSettings.Mask != CurrentState.Stencil.Mask )
        {
        glStencilMask ( NewSettings.FunctionMask );
        }

    if ( ( NewSettings.FunctionReference != CurrentState.Stencil.FunctionReference ) ||
            ( NewSettings.FunctionMask != CurrentState.Stencil.FunctionMask ) ||
            ( NewSettings.Function != CurrentState.Stencil.Function ) )
        {
        glStencilFunc ( Translate ( NewSettings.Function ), NewSettings.FunctionReference, NewSettings.FunctionMask );

        }

    if ( ( NewSettings.OnFail != CurrentState.Stencil.OnFail ) ||
            ( NewSettings.OnFailZ != CurrentState.Stencil.OnFailZ ) ||
            ( NewSettings.OnPassZ != CurrentState.Stencil.OnPassZ ) )
        {
        glStencilOp ( Translate ( NewSettings.OnFail ), Translate ( NewSettings.OnFailZ ), Translate ( NewSettings.OnPassZ ) );
        }

    CurrentState.Stencil = NewSettings;
    CheckError();
    }

void StateCache::ConfigureScissor ( const ScissorSettings &NewSettings )
    {
    if ( CurrentState.Scissor == NewSettings )
        return;
    if ( CurrentState.Scissor.Enabled != NewSettings.Enabled )
        {
        if ( NewSettings.Enabled )
            glEnable ( GL_SCISSOR_TEST );
        else
            glDisable ( GL_SCISSOR_TEST );
        CurrentState.Scissor.Enabled = NewSettings.Enabled;
        }
    if ( CurrentState.Scissor.Enabled == false )
        return;

    if ( ( CurrentState.Scissor.LowerLeft != NewSettings.LowerLeft ) ||
            ( CurrentState.Scissor.Dimensions != NewSettings.Dimensions ) )
        {
        glScissor ( NewSettings.LowerLeft.x, NewSettings.LowerLeft.y, NewSettings.Dimensions.x, NewSettings.Dimensions.y );
        }
    CurrentState.Scissor = NewSettings;
//    LOG_DEBUG ( "Setting scissor to %d %d %d %d", CurrentState.Scissor.LowerLeft.x, CurrentState.Scissor.LowerLeft.y, CurrentState.Scissor.Dimensions.x, CurrentState.Scissor.Dimensions.y );
    CheckError();
    }

void StateCache::ConfigureViewport ( const ViewportSettings &NewSettings )
    {
    if ( NewSettings.Enabled == false )
        {
        ViewportSettings Fullscreen;
        Fullscreen.Set ( glm::uvec2 ( 0 ), DefaultViewportSize );
        ConfigureViewport ( Fullscreen );
        return;
        }
    if ( CurrentState.Viewport == NewSettings )
        return;

    CurrentState.Viewport = NewSettings;
//    LOG_DEBUG ( "Setting viewport to %d %d %d %d", CurrentState.Viewport.LowerLeft.x, CurrentState.Viewport.LowerLeft.y, CurrentState.Viewport.Dimensions.x, CurrentState.Viewport.Dimensions.y );
    glViewport ( CurrentState.Viewport.LowerLeft.x, CurrentState.Viewport.LowerLeft.y, CurrentState.Viewport.Dimensions.x, CurrentState.Viewport.Dimensions.y );
    CheckError();
    }

void StateCache::ConfigureDepthTest ( const DepthTestSettings &NewSettings )
    {
    if ( CurrentState.DepthTest == NewSettings )
        return;
    if ( CurrentState.DepthTest.Enabled != NewSettings.Enabled )
        {
        if ( NewSettings.Enabled )
            glEnable ( GL_DEPTH_TEST );
        else
            glDisable ( GL_DEPTH_TEST );
        CurrentState.DepthTest.Enabled = NewSettings.Enabled;
        }
    if ( CurrentState.DepthTest.Enabled == false )
        return;

    glDepthFunc ( Translate ( NewSettings.Mode ) );
    CurrentState.DepthTest = NewSettings;
    CheckError();
    }
}
}
