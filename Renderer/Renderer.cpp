#include <CrossRendererConfig.hpp>
#include "Renderer.hpp"
#include "../Internal/Logger.hpp"
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT)
#include "OpenGL/OpenGLRenderer.hpp"
#endif

namespace CrossRenderer
{
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return (*name) ( __VA_ARGS__ ) = 0

DECLARE_INTERFACE_FUNCTION ( bool, InitializeRenderer, const RendererConfiguration &NewConfiguration );
DECLARE_INTERFACE_FUNCTION ( bool, ShutdownRenderer, void );

DECLARE_INTERFACE_FUNCTION ( bool, StartRenderToFramebuffer, const FramebufferHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( bool, DisplayFramebuffer, const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );

DECLARE_INTERFACE_FUNCTION ( bool, StartRenderToWindow, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( bool, DisplayWindow, const RenderWindowHandle &Handle );

DECLARE_INTERFACE_FUNCTION ( bool, RunCommand, const RenderCommand &Command );

// Shader buffer
DECLARE_INTERFACE_FUNCTION ( ShaderBufferHandle, CreateShaderBuffer, const ShaderBufferDescriptor CreationParameters );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteShaderBuffer, const ShaderBufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( bool, ChangeShaderBufferContents, const ShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize );
DECLARE_INTERFACE_FUNCTION ( void *, MapShaderBuffer, const ShaderBufferHandle Handle, const ShaderBufferMapAccessType AccessType );
DECLARE_INTERFACE_FUNCTION ( bool, UnmapShaderBuffer, const ShaderBufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( const ShaderBufferDescriptor, GetShaderBufferDescriptor, const ShaderBufferHandle Handle );

// Texture
DECLARE_INTERFACE_FUNCTION ( TextureHandle, CreateTexture, const TextureDescriptor CreationParameters );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteTexture, const TextureHandle Handle );
DECLARE_INTERFACE_FUNCTION ( bool, Load2DTextureData, const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data );
DECLARE_INTERFACE_FUNCTION ( bool, Update2DTextureRegion, const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data );
DECLARE_INTERFACE_FUNCTION ( bool, LoadCubeMapTextureData, const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] );
DECLARE_INTERFACE_FUNCTION ( glm::uvec2, GetTextureDimensions, const TextureHandle Handle );
DECLARE_INTERFACE_FUNCTION ( PixelFormat, GetTextureFormat, const TextureHandle Handle );
DECLARE_INTERFACE_FUNCTION ( TextureDescriptor, GetTextureDescriptor, const TextureHandle Handle );

// Shader
DECLARE_INTERFACE_FUNCTION ( ShaderHandle, CreateShader, const ShaderCode &NewCode );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteShader, const ShaderHandle Handle );
DECLARE_INTERFACE_FUNCTION ( void, GetShaderUniformList, const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList );
DECLARE_INTERFACE_FUNCTION ( ShaderUniformHandle, GetShaderUniformHandle, const ShaderHandle Handle, const std::string Name );
DECLARE_INTERFACE_FUNCTION ( void, GetShaderAttributeList, const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList );
DECLARE_INTERFACE_FUNCTION ( ShaderAttributeHandle, GetShaderAttributeHandle, const ShaderHandle Handle, const std::string Name );
DECLARE_INTERFACE_FUNCTION ( void, GetShaderInformation, const ShaderHandle Handle, ShaderInformation &Information );

// Framebuffer
DECLARE_INTERFACE_FUNCTION ( FramebufferHandle, CreateFramebuffer, const FramebufferDescriptor CreationParameters );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteFramebuffer, const FramebufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetFramebufferClearColor, const FramebufferHandle Handle, const glm::vec4 ClearColor );
DECLARE_INTERFACE_FUNCTION ( void, SetFramebufferClearDepth, const FramebufferHandle Handle, const float ClearDepth );
DECLARE_INTERFACE_FUNCTION ( void, ClearFramebuffer, const FramebufferHandle &Handle, const bool ShouldClearColorBuffer, const glm::vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask );
DECLARE_INTERFACE_FUNCTION ( glm::uvec2, GetFramebufferSize, const FramebufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( TextureHandle, GetColorBufferFromFramebuffer, const FramebufferHandle Handle, const size_t Index );
DECLARE_INTERFACE_FUNCTION ( TextureHandle, GetDepthBufferFromFramebuffer, const FramebufferHandle Handle );

#undef DECLARE_INTERFACE_FUNCTION

void CreateNewRenderer ( const RendererBackend &Backend )
	{
	switch ( Backend )
		{
#if defined (CROSS_RENDERER_OPENGL_CORE_SUPPORT)
		case RendererBackend::OpenGLCore:
#define ATTRIB(x) x = OpenGL::x
			ATTRIB ( InitializeRenderer );
			ATTRIB ( ShutdownRenderer );
			ATTRIB ( StartRenderToFramebuffer );
			ATTRIB ( DisplayFramebuffer );
			ATTRIB ( StartRenderToWindow );
			ATTRIB ( DisplayWindow );
			ATTRIB ( RunCommand );

			ATTRIB ( CreateShaderBuffer );
			ATTRIB ( DeleteShaderBuffer );
			ATTRIB ( ChangeShaderBufferContents );
			ATTRIB ( MapShaderBuffer );
			ATTRIB ( UnmapShaderBuffer );
			ATTRIB ( GetShaderBufferDescriptor );

			ATTRIB ( CreateTexture );
			ATTRIB ( DeleteTexture );
			ATTRIB ( Load2DTextureData );
			ATTRIB ( Update2DTextureRegion );
			ATTRIB ( LoadCubeMapTextureData );
			ATTRIB ( GetTextureDimensions );
			ATTRIB ( GetTextureFormat );
			ATTRIB ( GetTextureDimensions );

			ATTRIB ( CreateShader );
			ATTRIB ( DeleteShader );
			ATTRIB ( GetShaderUniformList );
			ATTRIB ( GetShaderUniformHandle );
			ATTRIB ( GetShaderAttributeList );
			ATTRIB ( GetShaderAttributeHandle );
			ATTRIB ( GetShaderInformation );

			ATTRIB ( CreateFramebuffer );
			ATTRIB ( DeleteFramebuffer );
			ATTRIB ( SetFramebufferClearColor );
			ATTRIB ( SetFramebufferClearDepth );
			ATTRIB ( ClearFramebuffer );
			ATTRIB ( GetFramebufferSize );
			ATTRIB ( GetColorBufferFromFramebuffer );
			ATTRIB ( GetDepthBufferFromFramebuffer );
#undef ATTRIB
			break;
#endif
		default:
			LOG_ERROR ( "Renderer backend '%s' is not built", Stringify ( Backend ) );
			return;
		}
	}

bool SanityCheckRenderCommand ( const RenderCommand& Command )
    {
    ShaderInformation Info;
    bool Result = true;
    if ( !Command.Shader )
        {
        LOG_ERROR ( "Invalid shader handle" );
        return false;
        }
    GetShaderInformation ( Command.Shader, Info );

    // Find invalid uniform handles
    for ( auto &Iterator : Command.UniformValues )
        {
        if ( Iterator.UniformHandle == ShaderUniformHandle::Invalid )
            {
            LOG_ERROR ( "Invalid uniform handle" );
            Result = false;
            break;
            }
        }

	// Find invalid attribute handles
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        if ( Iterator.AttributeHandle == ShaderAttributeHandle::Invalid )
            {
            LOG_ERROR ( "Invalid attribute handle" );
            Result = false;
            break;
            }
        }

	// Find invalid texture uniform handles
    for ( auto &Iterator : Command.TextureBindings )
        {
        if ( Iterator.UniformHandle == ShaderUniformHandle::Invalid )
            {
            LOG_ERROR ( "Invalid texture uniform handle" );
            Result = false;
            break;
            }
        }

    // Find duplicate uniform values
    auto FindUniformIndexFromHandle = [&Info] ( ShaderUniformHandle Handle )->int
        {
        for ( unsigned Iterator = 0; Iterator < Info.Uniforms.size(); ++Iterator )
            {
            if ( Info.Uniforms[Iterator].Handle == Handle )
                return ( int ) Iterator;
            }
        return -1;
        };
    for ( unsigned Iterator = 0; Iterator < Command.UniformValues.size(); ++Iterator )
        {
        int Index = FindUniformIndexFromHandle ( Command.UniformValues[Iterator].UniformHandle );
        if ( Index == -1 )
            {
            LOG_ERROR ( "Shader uniform handle does not belong to shader" );
            Result = false;
            continue;
            }
        for ( unsigned Iterator2 = Iterator + 1; Iterator2 < Command.UniformValues.size(); ++Iterator2 )
            {
            if ( Command.UniformValues[Iterator].UniformHandle == Command.UniformValues[Iterator2].UniformHandle )
                {
                LOG_ERROR ( "Duplicate value for uniform '%s'", Info.Uniforms[Index].Name.c_str() );
                Result = false;
                break;
                }
            }
        }

    // Find duplicate attribute bindings
    auto FindAttributeIndexFromHandle = [&Info] ( ShaderAttributeHandle Handle )->int
        {
        for ( unsigned Iterator = 0; Iterator < Info.Attributes.size(); ++Iterator )
            {
            if ( Info.Attributes[Iterator].Handle == Handle )
                return Iterator;
            }
        return -1;
        };
    for ( unsigned Iterator = 0; Iterator < Command.ShaderBufferBindings.size(); ++Iterator )
        {
        int Index = FindAttributeIndexFromHandle ( Command.ShaderBufferBindings[Iterator].AttributeHandle );
        if ( Index == -1 )
            {
            LOG_ERROR ( "Shader attribute handle does not belong to shader" );
            Result = false;
            continue;
            }
        for ( unsigned Iterator2 = Iterator + 1; Iterator2 < Command.ShaderBufferBindings.size(); ++Iterator2 )
            {
            if ( Command.ShaderBufferBindings[Iterator].AttributeHandle == Command.ShaderBufferBindings[Iterator2].AttributeHandle )
                {
                LOG_ERROR ( "Duplicate binding for attribute '%s'", Info.Attributes[Index].Name.c_str() );
                Result = false;
                break;
                }
            }
        }

    // Find missing textures
    for ( auto &Iterator : Command.TextureBindings )
        {
        if ( !Iterator.BindSettings.Handle )
            {
            LOG_ERROR ( "Missing texture handle for binding '%s'.", Info.Uniforms[Iterator.UniformHandle.GetKey()].Name.c_str() );
            Result = false;
            }
        }

    // Find missing uniform values
    for ( auto &Iterator : Command.UniformValues )
        {
        int Index = FindUniformIndexFromHandle ( Iterator.UniformHandle );
        if ( Index == -1 )
            continue;
        Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
        }
    for ( auto &Iterator : Command.TextureBindings )
        {
        int Index = FindUniformIndexFromHandle ( Iterator.UniformHandle );
        if ( Index == -1 )
            continue;
        Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
        }
    for ( unsigned Iterator = 0; Iterator < Info.Uniforms.size(); ++Iterator )
        {
		LOG_ERROR ( "Missing value for uniform '%s'. Type '%s'", Info.Uniforms[Iterator].Name.c_str(), Stringify ( Info.Uniforms[Iterator].Type ) );
        Result = false;
        }
    // Find missing attribute bindings
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        int Index = FindAttributeIndexFromHandle ( Iterator.AttributeHandle );
        if ( Index == -1 )
            continue;
        Info.Attributes.erase ( Info.Attributes.begin() + Index );
        }
    for ( unsigned Iterator = 0; Iterator < Info.Attributes.size(); ++Iterator )
        {
		LOG_ERROR ( "Missing binding for attribute '%s'. Type '%s'", Info.Attributes[Iterator].Name.c_str(), Stringify ( Info.Attributes[Iterator].Type ) );
        Result = false;
        }
    return Result;
    }
}
