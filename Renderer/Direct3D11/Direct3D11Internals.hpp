#pragma once
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler

#include "../RendererDefinitions.hpp"
#include "../ShaderBufferDefinitions.hpp"
#include "../TextureDefinitions.hpp"
#include "../ShaderDefinitions.hpp"
#include "../FramebufferDefinitions.hpp"
#include "../Version.hpp"
#include "../Internal/VectorizedContainer.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
struct D3D11InformationStruct
	{
	ID3D11Device *DevicePointer = nullptr;
	ID3D11DeviceContext *DeviceContextPointer = nullptr;
	IDXGISwapChain *swap_chain_ptr = NULL;
	};

extern D3D11InformationStruct D3D11Information;

struct UniformInfo
	{
	//GLint OpenGLID;
	ShaderUniformValue CurrentValue;
	ShaderUniformType Type;
	std::string Name;
	};

struct AttributeInfo
	{
	//GLint OpenGLID;
	bool Enabled;
	ShaderAttributeType Type;
	std::string Name;
	};

struct ShaderInfo
	{
	//GLuint OpenGLID;
	std::vector <ShaderObjectHandle> AttachedShaderObjects;
	std::vector <UniformInfo> Uniforms;
	std::vector <AttributeInfo> Attributes;
	};

struct ShaderBufferInfo : public ShaderBufferDescriptor
	{
	ID3D11Buffer *BufferPointer;
	void *MappedPointer;
	ShaderBufferInfo &operator = ( const ShaderBufferDescriptor &other )
		{
		*( (ShaderBufferDescriptor *) this ) = other;
		return *this;
		}
	};

struct TextureInfo : public TextureDescriptor
	{
	//GLuint OpenGLID;
	//GLint GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
	TextureInfo &operator = ( const TextureDescriptor &other )
		{
		*( (TextureDescriptor *) this ) = other;
		return *this;
		}
	};

struct FramebufferInfo : public FramebufferDescriptor
	{
	ID3D11Texture2D *framebuffer = nullptr;
	ID3D11RenderTargetView *render_target_view_ptr = nullptr;
	std::vector <TextureHandle> ColorTextures;
	TextureHandle DepthTexture;
	TextureHandle StencilTexture;
	FramebufferInfo &operator = ( const FramebufferDescriptor &other )
		{
		*( (FramebufferDescriptor *) this ) = other;
		return *this;
		}
	};

struct ShaderObjectInfo
	{
	//GLuint OpenGLID;
	std::string Code;
	ShaderObjectType Type;
	};

extern FramebufferHandle CurrentBoundFramebuffer;
extern VectorizedContainer <ShaderBufferInfo> ShaderBuffers;
extern VectorizedContainer <TextureInfo> Textures;
extern VectorizedContainer <ShaderInfo> Shaders;
extern VectorizedContainer <ShaderObjectInfo> ShaderObjects;
extern VectorizedContainer <FramebufferInfo> Framebuffers;
}
}
