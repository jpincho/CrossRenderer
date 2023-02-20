#pragma once
#include "RendererDefinitions.hpp"
#include "ShaderBufferDefinitions.hpp"
#include "TextureDefinitions.hpp"
#include "ShaderDefinitions.hpp"
#include "FramebufferDefinitions.hpp"
#include "RenderCommandDefinitions.hpp"
#include "Stringify.hpp"

namespace CrossRenderer
{
// Tricking intellisense into thinking these are actual functions. Very useful
#if defined (__INTELLISENSE__)
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return name ( __VA_ARGS__ )
#else
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
extern return (*name) ( __VA_ARGS__ )
#endif

DECLARE_INTERFACE_FUNCTION ( bool, InitializeRenderer, const RendererConfiguration &NewConfiguration );
DECLARE_INTERFACE_FUNCTION ( bool, ShutdownRenderer, void );

DECLARE_INTERFACE_FUNCTION ( bool, StartRenderToFramebuffer, const FramebufferHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( bool, DisplayFramebuffer, const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );

DECLARE_INTERFACE_FUNCTION ( bool, StartFrame, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( bool, EndFrame, const RenderWindowHandle &Handle );

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
void CreateNewRenderer ( const RendererBackend &Backend );
bool SanityCheckRenderCommand ( const RenderCommand &Command );
}
