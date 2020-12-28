#pragma once
#include <CrossRendererConfig.h>
#include "RendererDefinitions.h"
#include "ShaderBufferDefinitions.h"
#include "TextureDefinitions.h"
#include "ShaderDefinitions.h"
#include "FramebufferDefinitions.h"
#include "RenderCommandDefinitions.h"
#include <Logger/Logger.h>
#include "Stringify.h"

namespace CrossRenderer
{
extern bool ( *InitializeRenderer ) ( const RendererConfiguration &NewConfiguration );
extern bool ( *ShutdownRenderer ) ( void );

extern bool ( *StartRenderToFramebuffer ) ( const FramebufferHandle &Handle );
extern bool ( *DisplayFramebuffer ) ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );

extern bool ( *StartFrame ) ( const RenderWindowHandle &Handle );
extern bool ( *EndFrame ) ( const RenderWindowHandle &Handle );

extern bool ( *RunCommand ) ( const RenderCommand &Command );

extern ShaderBufferHandle ( *CreateShaderBuffer ) ( const ShaderBufferDescriptor CreationParameters );
extern bool ( *DeleteShaderBuffer ) ( const ShaderBufferHandle Handle );
extern bool ( *ChangeShaderBufferContents ) ( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters );

extern TextureHandle ( *CreateTexture ) ( const TextureDescriptor CreationParameters );
extern bool ( *DeleteTexture ) ( const TextureHandle Handle );
extern bool ( *Load2DTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data, const unsigned Pitch );
extern bool ( *UpdateTextureRegion ) ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const void *Data );
extern glm::uvec2 ( *GetTextureDimensions ) ( const TextureHandle Handle );

extern ShaderHandle ( *CreateShader ) ( const ShaderCode &NewCode );
extern bool ( *DeleteShader ) ( const ShaderHandle Handle );
extern bool ( *ChangeShaderCode ) ( const ShaderHandle Handle, const ShaderCode &NewCode );
extern void ( *GetShaderUniformList ) ( const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList );
extern ShaderUniformHandle ( *GetShaderUniformHandle ) ( const ShaderHandle Handle, const std::string Name );
extern void ( *GetShaderAttributeList ) ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList );
extern ShaderAttributeHandle ( *GetShaderAttributeHandle ) ( const ShaderHandle Handle, const std::string Name );
extern void ( *GetShaderInformation ) ( const ShaderHandle Handle, ShaderInformation &Information );

extern FramebufferHandle ( *CreateFramebuffer ) ( const FramebufferDescriptor CreationParameters );
extern bool ( *DeleteFramebuffer ) ( const FramebufferHandle Handle );
extern glm::uvec2 ( *GetFramebufferSize ) ( const FramebufferHandle Handle );
extern TextureHandle ( *GetColorBufferFromFramebuffer ) ( const FramebufferHandle Handle, const size_t Index );
extern TextureHandle ( *GetDepthBufferFromFramebuffer ) ( const FramebufferHandle Handle );

bool SanityCheckRenderCommand ( const RenderCommand &Command );
}
