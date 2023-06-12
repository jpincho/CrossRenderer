#include "../FramebufferDefinitions.hpp"
#include "../TextureDefinitions.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
FramebufferHandle CreateFramebuffer ( const FramebufferDescriptor CreationParameters );
bool DeleteFramebuffer ( const FramebufferHandle Handle );
void SetFramebufferClearColor ( const FramebufferHandle Handle, const glm::vec4 ClearColor );
void SetFramebufferClearDepth ( const FramebufferHandle Handle, const float ClearDepth );
void ClearFramebuffer ( const FramebufferHandle &Handle, const bool ShouldClearColorBuffer, const glm::vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask );
glm::uvec2 GetFramebufferSize ( const FramebufferHandle Handle );
TextureHandle GetColorBufferFromFramebuffer ( const FramebufferHandle Handle, const size_t Index = 0 );
TextureHandle GetDepthBufferFromFramebuffer ( const FramebufferHandle Handle );
}
}
