#include "OpenGLRenderer.h"
#include "OpenGLContext.h"
#include "OpenGLInternals.h"
#include "OpenGLStateCache.h"
#include "../WindowManager/WindowManager.h"
#include <Logger/Logger.h>
#include "../Renderer.h"

namespace CrossRenderer
{
namespace OpenGL
{
RendererConfiguration Configuration;
OpenGLContextInterface *Context;
unsigned GeneralVAO;
std::string OpenGLRendererString, OpenGLVendor[3];
int MaxTextureUnits;
ShaderHandle ActiveShader;
RenderWindowHandle ActiveWindow;
StateCache CurrentState;
bool SetShaderUniformValue ( const ShaderHandle Handle, const ShaderUniformHandle UniformHandle, const ShaderUniformValue &Value );

bool InitializeRenderer ( const RendererConfiguration &NewConfiguration )
    {
    Configuration = NewConfiguration;
    Context = GetOpenGLContext ( NewConfiguration.DesiredWindowBackend );

    for ( auto &Iterator : NewConfiguration.InitialWindowConfigurations )
        {
        RenderWindowHandle NewWindow = WindowManager::CreateNewWindow ( Iterator );
        if ( !NewWindow )
            return false;
        }
    ActiveWindow = * ( WindowManager::WindowList.begin() );
    if ( Context->Create ( Configuration, ActiveWindow ) == false )
        goto OnError;

    Context->MakeActive ( ActiveWindow );
    if ( Context->GetOpenGLVersion().Lesser ( 3, 0 ) )
        {
        LOG_ERROR ( "OpenGL Core 3.0+ needed" );
        return false;
        }

    // If necessary, create a VAO
    glGenVertexArrays ( 1, &GeneralVAO );
    glBindVertexArray ( GeneralVAO );

    CurrentState.Invalidate();

    glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxTextureUnits );
    glClearColor ( 0, 0, 0, 0 );
    glClearDepth ( 1.0 );

    for ( GLenum iterator = 0; iterator < /*std::numeric_limits<unsigned>::max ()*/3; ++iterator )
        {
        const char *String = reinterpret_cast<const char *> ( glGetString ( GL_VENDOR + iterator ) );
        if ( ( String != nullptr ) && ( strlen ( String ) > 0 ) )
            OpenGLVendor[iterator].assign ( String );
        else
            break;
        }
        {
        const char *String = reinterpret_cast<const char *> ( glGetString ( GL_RENDERER ) );
        if ( ( String != nullptr ) && ( strlen ( String ) > 0 ) )
            OpenGLRendererString.assign ( String );
        }

    if ( CheckError() == false )
        goto OnError;

    //    for ( auto &iterator : OpenGLVendor )
    //        LOG_DEBUG ( "OpenGL vendor information: %s", iterator.c_str () );

    LOG_DEBUG ( "Max Texture Units - %u", MaxTextureUnits );

    glEnable ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback ( OpenGLMessageCallback, nullptr );
    return true;
OnError:
    if ( GeneralVAO != 0 )
        {
        glDeleteVertexArrays ( 1, &GeneralVAO );
        GeneralVAO = 0;
        }
    if ( Context )
        {
        Context->Destroy();
        delete Context;
        Context = nullptr;
        }
    return false;
    }

bool ShutdownRenderer ( void )
    {
    if ( GeneralVAO != 0 )
        {
        glDeleteVertexArrays ( 1, &GeneralVAO );
        GeneralVAO = 0;
        }
    if ( Context )
        {
        Context->Destroy();
        delete Context;
        Context = nullptr;
        }
    return true;
    }

bool StartRenderToFramebuffer ( const FramebufferHandle &Handle )
    {
    FramebufferInfo *FramebufferInformation = &Framebuffers[Handle];

    CurrentState.SetDefaultViewportSize ( FramebufferInformation->Dimensions );
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings() );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings() );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings() );

    glBindFramebuffer ( GL_FRAMEBUFFER, FramebufferInformation->OpenGLID );
    int BitMask = 0;
    if ( FramebufferInformation->ColorTextures.size() )
        {
        BitMask |= GL_COLOR_BUFFER_BIT;
        glClearColor ( FramebufferInformation->ClearColor.r, FramebufferInformation->ClearColor.g, FramebufferInformation->ClearColor.b, FramebufferInformation->ClearColor.a );
        }
    if ( FramebufferInformation->DepthTexture )
        {
        BitMask |= GL_DEPTH_BUFFER_BIT;
        glClearDepth ( FramebufferInformation->ClearDepth );
        }
    if ( BitMask )
        glClear ( BitMask );
    return CheckError();
    }

bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle )
    {
    ActiveWindow = WindowHandle;
    Context->MakeActive ( WindowHandle );
    glm::uvec2 WindowSize = WindowManager::GetWindowSize ( WindowHandle );
    CurrentState.SetDefaultViewportSize ( WindowSize );
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings() );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings() );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings() );

    FramebufferInfo *FramebufferInformation = &Framebuffers[Handle];
    glBindFramebuffer ( GL_READ_FRAMEBUFFER, FramebufferInformation->OpenGLID );
    glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, 0 );
    glBlitFramebuffer (
        0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
        0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
        GL_COLOR_BUFFER_BIT, GL_NEAREST );
    return CheckError();
    }

bool StartFrame ( const RenderWindowHandle &Handle )
    {
    ActiveWindow = Handle;
    CurrentState.SetDefaultViewportSize ( WindowManager::GetWindowSize ( Handle ) );
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings() );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings() );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings() );

    Context->MakeActive ( Handle );
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    return true;
    }

bool EndFrame ( const RenderWindowHandle &Handle )
    {
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings() );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings() );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings() );
    Context->SwapWindowBuffer ( Handle );
    return true;
    }

bool RunCommand ( const RenderCommand &Command )
    {
    // Disable active vertex attributes
    if ( ( ActiveShader ) && ( ActiveShader != Command.Shader ) )
        {
        ShaderInfo *OldShaderInfo = &Shaders[ActiveShader];
        for ( auto &AttributeIterator : OldShaderInfo->Attributes )
            {
            if ( AttributeIterator.Enabled == true )
                {
                AttributeIterator.Enabled = false;
                glDisableVertexAttribArray ( AttributeIterator.OpenGLID );
                }
            }
        }
    // Activate new shader
    ShaderInfo *ShaderInformation = &Shaders[Command.Shader];
    glUseProgram ( ShaderInformation->OpenGLID );
    ActiveShader = Command.Shader;
    CheckError();

    CurrentState.ApplyState ( Command.State );
    CheckError();
    for ( auto &Iterator : Command.UniformValues )
        {
        if ( !Iterator.UniformHandle )
            return false;
        UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.key()];

        if ( UniformInformation->CurrentValue.Equals ( Iterator.UniformValue, UniformInformation->Type ) == true )
            continue;
        SetShaderUniformValue ( Command.Shader, Iterator.UniformHandle, Iterator.UniformValue );
        UniformInformation->CurrentValue = Iterator.UniformValue;
        }
    CheckError();
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        AttributeInfo *AttributeInformation = & ( ShaderInformation->Attributes[Iterator.AttributeHandle.key()] );
        ShaderBufferInfo *BufferInformation = & ( ShaderBuffers[Iterator.DataStream.BufferHandle] );

        if ( AttributeInformation->Enabled == false )
            {
            glEnableVertexAttribArray ( AttributeInformation->OpenGLID );
            AttributeInformation->Enabled = true;
            }

        if ( BufferInformation->MappedPointer )
            {
            UnmapShaderBuffer ( Iterator.DataStream.BufferHandle );
            }

        glBindBuffer ( GL_ARRAY_BUFFER, BufferInformation->OpenGLID );
        glVertexAttribPointer ( AttributeInformation->OpenGLID,
                                ( GLint ) Iterator.DataStream.ComponentsPerElement,
                                Translate ( Iterator.DataStream.ComponentType ),
                                Iterator.DataStream.NormalizeData,
                                ( GLsizei ) Iterator.DataStream.Stride,
                                ( void * ) Iterator.DataStream.StartOffset );
        }
    CheckError();
    unsigned TextureLevel = 0;
    for ( auto &Iterator : Command.TextureBindings )
        {
        UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.key()];
        //if ( UniformInformation->Type != ShaderUniformType::Sampler2D ) return false;

        GLint DesiredSWrap = Translate ( Iterator.BindSettings.WrapSettings.Horizontal );
        GLint DesiredTWrap = Translate ( Iterator.BindSettings.WrapSettings.Vertical );
        GLint DesiredMinFilter = Translate ( Iterator.BindSettings.FilterSettings.MinFilter );
        GLint DesiredMagFilter = Translate ( Iterator.BindSettings.FilterSettings.MagFilter );

        TextureInfo *TextureInformation = &Textures[Iterator.BindSettings.Handle];
        glActiveTexture ( GL_TEXTURE0 + TextureLevel ); // Multitexture index
        glBindTexture ( GL_TEXTURE_2D, TextureInformation->OpenGLID );
        if ( TextureInformation->GLSWrap != DesiredSWrap )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DesiredSWrap );
        if ( TextureInformation->GLTWrap != DesiredTWrap )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DesiredTWrap );
        if ( TextureInformation->GLMinFilter != DesiredMinFilter )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DesiredMinFilter );
        if ( TextureInformation->GLMagFilter != DesiredMagFilter )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DesiredMagFilter );
        glUniform1i ( UniformInformation->OpenGLID, ( GLint ) TextureLevel );

        if ( CheckError() == false )
            {
            LOG_ERROR ( "Error activating texture" );
            return false;
            }
        TextureInformation->GLSWrap = DesiredSWrap;
        TextureInformation->GLTWrap = DesiredTWrap;
        TextureInformation->GLMinFilter = DesiredMinFilter;
        TextureInformation->GLMagFilter = DesiredMagFilter;

        ++TextureLevel;
        }
    CheckError();

    if ( Command.IndexBuffer )
        {
        static size_t Sizes[] = { sizeof ( float ), sizeof ( uint8_t ), sizeof ( uint16_t ), sizeof ( uint32_t ) };

        ShaderBufferInfo *BufferToUse = &ShaderBuffers[Command.IndexBuffer];
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, BufferToUse->OpenGLID );
        glDrawElements ( Translate ( Command.Primitive ),
                         ( GLint ) Command.VertexCount,
                         Translate ( Command.IndexBufferStream.ComponentType ),
                         reinterpret_cast <void *> ( Command.StartVertex * Sizes[ ( int ) Command.IndexBufferStream.ComponentType] ) );
        }
    else
        {
        glDrawArrays ( Translate ( Command.Primitive ), ( GLint ) Command.StartVertex, static_cast <GLsizei> ( Command.VertexCount ) );
        }
    return CheckError();
    }
}
}
