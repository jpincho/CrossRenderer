#include "Renderer.h"

namespace CrossRenderer
{
bool ( *InitializeRenderer ) ( const RendererConfiguration &NewConfiguration ) = 0;
bool ( *ShutdownRenderer ) ( void ) = 0;

bool ( *StartRenderToFramebuffer ) ( const FramebufferHandle &Handle ) = 0;
bool ( *DisplayFramebuffer ) ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle ) = 0;

bool ( *StartFrame ) ( const RenderWindowHandle &Handle ) = 0;
bool ( *EndFrame ) ( const RenderWindowHandle &Handle ) = 0;

bool ( *RunCommand ) ( const RenderCommand &Command ) = 0;

ShaderBufferHandle ( *CreateShaderBuffer ) ( const ShaderBufferDescriptor CreationParameters ) = 0;
bool ( *DeleteShaderBuffer ) ( const ShaderBufferHandle Handle ) = 0;
bool ( *ChangeShaderBufferContents ) ( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters ) = 0;

TextureHandle ( *CreateTexture ) ( const TextureDescriptor CreationParameters ) = 0;
bool ( *DeleteTexture ) ( const TextureHandle Handle ) = 0;
bool ( *Load2DTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data ) = 0;
bool ( *Update2DTextureRegion ) ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data ) = 0;
bool ( *LoadCubeMapTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] ) = 0;
glm::uvec2 ( *GetTextureDimensions ) ( const TextureHandle Handle ) = 0;

ShaderHandle ( *CreateShader ) ( const ShaderCode &NewCode ) = 0;
bool ( *DeleteShader ) ( const ShaderHandle Handle ) = 0;
bool ( *ChangeShaderCode ) ( const ShaderHandle Handle, const ShaderCode &NewCode ) = 0;
void ( *GetShaderUniformList ) ( const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList ) = 0;
ShaderUniformHandle ( *GetShaderUniformHandle ) ( const ShaderHandle Handle, const std::string Name ) = 0;
void ( *GetShaderAttributeList ) ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList ) = 0;
ShaderAttributeHandle ( *GetShaderAttributeHandle ) ( const ShaderHandle Handle, const std::string Name ) = 0;
void ( *GetShaderInformation ) ( const ShaderHandle Handle, ShaderInformation &Information ) = 0;

FramebufferHandle ( *CreateFramebuffer ) ( const FramebufferDescriptor CreationParameters ) = 0;
bool ( *DeleteFramebuffer ) ( const FramebufferHandle Handle ) = 0;
glm::uvec2 ( *GetFramebufferSize ) ( const FramebufferHandle Handle ) = 0;
TextureHandle ( *GetColorBufferFromFramebuffer ) ( const FramebufferHandle Handle, const size_t Index ) = 0;
TextureHandle ( *GetDepthBufferFromFramebuffer ) ( const FramebufferHandle Handle ) = 0;
bool SanityCheckRenderCommand ( const RenderCommand &Command );
}
