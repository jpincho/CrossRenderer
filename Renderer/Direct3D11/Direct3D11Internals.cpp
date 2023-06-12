#include "Direct3D11Internals.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
D3D11InformationStruct D3D11Information; 
VectorizedContainer <ShaderBufferInfo> ShaderBuffers;
VectorizedContainer <TextureInfo> Textures;
VectorizedContainer <ShaderInfo> Shaders;
VectorizedContainer <ShaderObjectInfo> ShaderObjects;
VectorizedContainer <FramebufferInfo> Framebuffers;
}
}
