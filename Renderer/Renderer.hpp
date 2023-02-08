#pragma once
#include <CrossRendererConfig.hpp>
#include "RendererDefinitions.hpp"
#include "ShaderBufferDefinitions.hpp"
#include "TextureDefinitions.hpp"
#include "ShaderDefinitions.hpp"
#include "FramebufferDefinitions.hpp"
#include "RenderCommandDefinitions.hpp"
#include "Stringify.hpp"
#include "../Logger.hpp"

namespace CrossRenderer
{
extern bool ( *InitializeRenderer ) ( const RendererConfiguration &NewConfiguration );
extern bool ( *ShutdownRenderer ) ( void );

extern bool ( *StartRenderToFramebuffer ) ( const FramebufferHandle &Handle );
extern bool ( *DisplayFramebuffer ) ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );

extern bool ( *StartFrame ) ( const RenderWindowHandle &Handle );
extern bool ( *EndFrame ) ( const RenderWindowHandle &Handle );

extern bool ( *RunCommand ) ( const RenderCommand &Command );

// Shader buffer
extern ShaderBufferHandle ( *CreateShaderBuffer ) ( const ShaderBufferDescriptor CreationParameters );
extern bool ( *DeleteShaderBuffer ) ( const ShaderBufferHandle Handle );
extern bool ( *ChangeShaderBufferContents )( const ShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize );
extern void *( *MapShaderBuffer )( const ShaderBufferHandle Handle, const ShaderBufferMapAccessType AccessType );
extern bool ( *UnmapShaderBuffer )( const ShaderBufferHandle Handle );
extern const ShaderBufferDescriptor ( *GetShaderBufferDescriptor )( const ShaderBufferHandle Handle );

// Texture
extern TextureHandle ( *CreateTexture ) ( const TextureDescriptor CreationParameters );
extern bool ( *DeleteTexture ) ( const TextureHandle Handle );
extern bool ( *Load2DTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data );
extern bool ( *Update2DTextureRegion ) ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data );
extern bool ( *LoadCubeMapTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] );
extern glm::uvec2 ( *GetTextureDimensions ) ( const TextureHandle Handle );
extern PixelFormat ( *GetTextureFormat )( const TextureHandle Handle );
extern TextureDescriptor ( *GetTextureDescriptor )( const TextureHandle Handle );

// Shader
extern ShaderHandle ( *CreateShader ) ( const ShaderCode &NewCode );
extern bool ( *DeleteShader ) ( const ShaderHandle Handle );
extern bool ( *ChangeShaderCode ) ( const ShaderHandle Handle, const ShaderCode &NewCode );
extern void ( *GetShaderUniformList ) ( const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList );
extern ShaderUniformHandle ( *GetShaderUniformHandle ) ( const ShaderHandle Handle, const std::string Name );
extern void ( *GetShaderAttributeList ) ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList );
extern ShaderAttributeHandle ( *GetShaderAttributeHandle ) ( const ShaderHandle Handle, const std::string Name );
extern void ( *GetShaderInformation ) ( const ShaderHandle Handle, ShaderInformation &Information );

// Framebuffer
extern FramebufferHandle ( *CreateFramebuffer ) ( const FramebufferDescriptor CreationParameters );
extern bool ( *DeleteFramebuffer ) ( const FramebufferHandle Handle );
extern void ( *SetFramebufferClearColor ) ( const FramebufferHandle Handle, const glm::vec4 ClearColor );
extern void ( *SetFramebufferClearDepth ) ( const FramebufferHandle Handle, const float ClearDepth );
extern void ( *ClearFramebuffer ) ( const FramebufferHandle &Handle, const bool ShouldClearColorBuffer, const glm::vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask );
extern glm::uvec2 ( *GetFramebufferSize )( const FramebufferHandle Handle );
extern TextureHandle ( *GetColorBufferFromFramebuffer ) ( const FramebufferHandle Handle, const size_t Index );
extern TextureHandle ( *GetDepthBufferFromFramebuffer ) ( const FramebufferHandle Handle );

void CreateNewRenderer ( const RendererBackend &Backend );
bool SanityCheckRenderCommand ( const RenderCommand &Command );
}
