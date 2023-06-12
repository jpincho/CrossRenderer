#include "Direct3D11Framebuffer.hpp"
#include "Direct3D11Internals.hpp"
#include "../Internal/Logger.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
FramebufferHandle CreateFramebuffer ( const FramebufferDescriptor  )
    {
	return FramebufferHandle::Invalid;
    }

bool DeleteFramebuffer ( const FramebufferHandle Handle )
    {
	//FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];

    Framebuffers.ReleaseIndex ( Handle.GetKey () );
    return true;
    }

void SetFramebufferClearColor ( const FramebufferHandle Handle, const glm::vec4 ClearColor )
	{
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];
	FramebufferInformation->ClearColor = ClearColor;
	}

void SetFramebufferClearDepth ( const FramebufferHandle Handle, const float ClearDepth )
	{
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];
	FramebufferInformation->ClearDepth = ClearDepth;
	}

void ClearFramebuffer ( const FramebufferHandle &, const bool , const glm::vec4 , const bool , const float , const bool , const int , const int  )
	{
	//FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];
	}

glm::uvec2 GetFramebufferSize ( const FramebufferHandle Handle )
    {
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];

	return FramebufferInformation->Dimensions;
    }

TextureHandle GetColorBufferFromFramebuffer ( const FramebufferHandle Handle, const size_t Index )
    {
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];

	if ( Index > FramebufferInformation->ColorTextures.size () )
		return TextureHandle::Invalid;
	return FramebufferInformation->ColorTextures[Index];
    }

TextureHandle GetDepthBufferFromFramebuffer ( const FramebufferHandle Handle )
    {
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];

	return FramebufferInformation->DepthTexture;
    }
}
}
