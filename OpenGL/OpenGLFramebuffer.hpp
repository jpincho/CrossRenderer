#include "../FramebufferDefinitions.hpp"
#include "../TextureDefinitions.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
FramebufferHandle CreateFramebuffer ( const FramebufferDescriptor CreationParameters );
bool DeleteFramebuffer ( const FramebufferHandle Handle );
glm::uvec2 GetFramebufferSize ( const FramebufferHandle Handle );
TextureHandle GetColorBufferFromFramebuffer ( const FramebufferHandle Handle, const size_t Index = 0 );
TextureHandle GetDepthBufferFromFramebuffer ( const FramebufferHandle Handle );
}
}
