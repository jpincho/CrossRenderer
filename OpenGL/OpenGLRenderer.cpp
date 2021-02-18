#include "OpenGLRenderer.h"
#include "OpenGLContext.h"
#include "OpenGLInternals.h"
#include "../WindowManager/WindowManager.h"
#include "../Renderer.h"

namespace CrossRenderer
{
namespace OpenGL
{
RendererConfiguration Configuration;
OpenGLContextInterface *Context;
unsigned GeneralVAO;
std::string OpenGLRendererString, OpenGLVendor[3];
ShaderHandle ActiveShader;
RenderWindowHandle ActiveWindow;

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
    ActiveWindow = * ( WindowManager::WindowList.begin () );
    if ( Context->Create ( Configuration, ActiveWindow ) == false )
        goto OnError;

    Context->MakeActive ( ActiveWindow );
    if ( Context->GetOpenGLVersion ().Lesser ( 3, 0 ) )
        {
        LOG_ERROR ( "OpenGL Core 3.0+ needed" );
        return false;
        }

    // If necessary, create a VAO
    glGenVertexArrays ( 1, &GeneralVAO );
    glBindVertexArray ( GeneralVAO );

    CurrentState.Invalidate ();

    glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &OpenGLInformation.MaxTextureUnits );
    OpenGLInformation.OpenGLVersion = Context->GetOpenGLVersion ();
    OpenGLInformation.GLSLVersion = Context->GetGLSLVersion ();

    glClearColor ( 0, 0, 0, 0 );
    glClearDepth ( 1.0 );

    if ( CheckError () == false )
        goto OnError;

    LOG_DEBUG ( "Max Texture Units - %u", OpenGLInformation.MaxTextureUnits );

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
        Context->Destroy ();
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
        Context->Destroy ();
        delete Context;
        Context = nullptr;
        }
    return true;
    }

bool StartRenderToFramebuffer ( const FramebufferHandle &Handle )
    {
    FramebufferInfo *FramebufferInformation = &Framebuffers[Handle];

    CurrentState.SetDefaultViewportSize ( FramebufferInformation->Dimensions );
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings () );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings () );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings () );

    glBindFramebuffer ( GL_FRAMEBUFFER, FramebufferInformation->OpenGLID );
    int BitMask = 0;
    if ( FramebufferInformation->ColorTextures.size () )
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
    return CheckError ();
    }

bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle )
    {
    ActiveWindow = WindowHandle;
    Context->MakeActive ( WindowHandle );
    glm::uvec2 WindowSize = WindowManager::GetWindowSize ( WindowHandle );
    CurrentState.SetDefaultViewportSize ( WindowSize );
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings () );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings () );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings () );

    FramebufferInfo *FramebufferInformation = &Framebuffers[Handle];
    glBindFramebuffer ( GL_READ_FRAMEBUFFER, FramebufferInformation->OpenGLID );
    glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, 0 );
    glBlitFramebuffer (
        0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
        0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
        GL_COLOR_BUFFER_BIT, GL_NEAREST );
    return CheckError ();
    }

bool StartFrame ( const RenderWindowHandle &Handle )
    {
    ActiveWindow = Handle;
    CurrentState.SetDefaultViewportSize ( WindowManager::GetWindowSize ( Handle ) );
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings () );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings () );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings () );

    Context->MakeActive ( Handle );
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    return true;
    }

bool EndFrame ( const RenderWindowHandle &Handle )
    {
    CurrentState.ConfigureScissor ( CrossRenderer::ScissorSettings () );
    CurrentState.ConfigureViewport ( CrossRenderer::ViewportSettings () );
    CurrentState.ConfigureStencil ( CrossRenderer::StencilBufferSettings () );
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
    CheckError ();

    CurrentState.ApplyState ( Command.State );
    CheckError ();

    // Set all uniforms
    unsigned UniformBlockBindingIndex = 0;
    for ( auto &Iterator : Command.UniformValues )
        {
        if ( !Iterator.UniformHandle )
            return false;
        UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.key ()];

        if ( UniformInformation->CurrentValue.Equals ( Iterator.UniformValue, UniformInformation->Type ) == true )
            continue;

        switch ( UniformInformation->Type )
            {
#define CASE_TYPE(TYPENAME,FUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( UniformInformation->OpenGLID, Iterator.UniformValue.TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( UniformInformation->OpenGLID, 1, glm::value_ptr(Iterator.UniformValue.TYPENAME##Value) );\
            break;\
            }

                CASE_TYPE ( Float, glUniform1f );
                CASE_TYPE_VEC ( Float2, glUniform2fv );
                CASE_TYPE_VEC ( Float3, glUniform3fv );
                CASE_TYPE_VEC ( Float4, glUniform4fv );

                CASE_TYPE ( Integer, glUniform1i );
                CASE_TYPE_VEC ( Integer2, glUniform2iv );
                CASE_TYPE_VEC ( Integer3, glUniform3iv );
                CASE_TYPE_VEC ( Integer4, glUniform4iv );

                CASE_TYPE ( UnsignedInteger, glUniform1ui );
                CASE_TYPE_VEC ( UnsignedInteger2, glUniform2uiv );
                CASE_TYPE_VEC ( UnsignedInteger3, glUniform3uiv );
                CASE_TYPE_VEC ( UnsignedInteger4, glUniform4uiv );
#undef CASE_TYPE
#undef CASE_TYPE_VEC

            case ShaderUniformType::Matrix3:
                glUniformMatrix3fv ( UniformInformation->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Iterator.UniformValue.Matrix3Value ) );
                break;
            case ShaderUniformType::Matrix4:
                glUniformMatrix4fv ( UniformInformation->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Iterator.UniformValue.Matrix4Value ) );
                break;
            case ShaderUniformType::Bool:
                glUniform1i ( UniformInformation->OpenGLID, Iterator.UniformValue.BoolValue );
                break;
            case ShaderUniformType::Block:
                {
                ShaderBufferInfo SBInfo = ShaderBuffers[Iterator.UniformValue.BlockValue];
                glUniformBlockBinding ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, UniformBlockBindingIndex );
                glBindBufferRange ( GL_UNIFORM_BUFFER, UniformBlockBindingIndex, SBInfo.OpenGLID, 0, SBInfo.DataSize );
                ++UniformBlockBindingIndex;
                break;
                }
            default:
                throw std::runtime_error ( "Unhandled shader uniform type" );
            }
        UniformInformation->CurrentValue = Iterator.UniformValue;
        }
    CheckError ();

    // Bind all attribute buffers
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        AttributeInfo *AttributeInformation = & ( ShaderInformation->Attributes[Iterator.AttributeHandle.key ()] );
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
    CheckError ();

    // Bind all textures
    unsigned TextureBindingIndex = 0;
    for ( auto &Iterator : Command.TextureBindings )
        {
        UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.key ()];
        //if ( UniformInformation->Type != ShaderUniformType::Sampler2D ) return false;

        GLint DesiredSWrap = Translate ( Iterator.BindSettings.WrapSettings.Horizontal );
        GLint DesiredTWrap = Translate ( Iterator.BindSettings.WrapSettings.Vertical );
        GLint DesiredMinFilter = Translate ( Iterator.BindSettings.FilterSettings.MinFilter );
        GLint DesiredMagFilter = Translate ( Iterator.BindSettings.FilterSettings.MagFilter );

        TextureInfo *TextureInformation = &Textures[Iterator.BindSettings.Handle];
        glActiveTexture ( GL_TEXTURE0 + TextureBindingIndex ); // Multitexture index
        glBindTexture ( GL_TEXTURE_2D, TextureInformation->OpenGLID );
        if ( TextureInformation->GLSWrap != DesiredSWrap )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DesiredSWrap );
        if ( TextureInformation->GLTWrap != DesiredTWrap )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DesiredTWrap );
        if ( TextureInformation->GLMinFilter != DesiredMinFilter )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DesiredMinFilter );
        if ( TextureInformation->GLMagFilter != DesiredMagFilter )
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DesiredMagFilter );
        glUniform1i ( UniformInformation->OpenGLID, ( GLint ) TextureBindingIndex );

        if ( CheckError () == false )
            {
            LOG_ERROR ( "Error activating texture" );
            return false;
            }
        TextureInformation->GLSWrap = DesiredSWrap;
        TextureInformation->GLTWrap = DesiredTWrap;
        TextureInformation->GLMinFilter = DesiredMinFilter;
        TextureInformation->GLMagFilter = DesiredMagFilter;

        ++TextureBindingIndex;
        }
    CheckError ();

    // Finally, issue the draw call
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
    return CheckError ();
    }
}
}
