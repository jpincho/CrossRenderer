#include "OpenGLFramebuffer.hpp"
#include "OpenGLInternals.hpp"
#include "OpenGLTexture.hpp"
#include "../Logger.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
FramebufferHandle CreateFramebuffer ( const FramebufferDescriptor CreationParameters )
    {
    FramebufferInfo NewFramebuffer;
    GLenum FramebufferStatus;

    NewFramebuffer = CreationParameters;

    if ( CreationParameters.DepthEnabled )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.Format = CreationParameters.DepthFormat;
        NewTextureDescriptor.Type = TextureType::Texture2D;
        NewFramebuffer.DepthTexture = CreateTexture ( NewTextureDescriptor );
        if ( !NewFramebuffer.DepthTexture )
            return FramebufferHandle::Invalid;
        }
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.Format = CreationParameters.ColorAttachmentFormat;
        NewTextureDescriptor.Type = TextureType::Texture2D;
        TextureHandle NewTexture = CreateTexture ( NewTextureDescriptor );
        if ( !NewTexture )
            goto error;
        NewFramebuffer.ColorTextures.push_back ( NewTexture );
        }

    glGenFramebuffers ( 1, &NewFramebuffer.OpenGLID );
    glBindFramebuffer ( GL_FRAMEBUFFER, NewFramebuffer.OpenGLID );
    if  ( CreationParameters.DepthEnabled )
        glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Textures[NewFramebuffer.DepthTexture.GetKey ()].OpenGLID, 0 );
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + cont, GL_TEXTURE_2D, Textures[NewFramebuffer.ColorTextures[cont].GetKey ()].OpenGLID, 0 );

    if ( CheckError() == false )
        goto error;

    FramebufferStatus = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
    glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

    if ( FramebufferStatus != GL_FRAMEBUFFER_COMPLETE )
        {
        LOG_ERROR ( "Unable to create framebuffer. %s", StringifyOpenGL ( FramebufferStatus ) );
        goto error;
        }

        {
        FramebufferHandle NewHandle ( Framebuffers.GetFreeIndex() );
        Framebuffers[NewHandle.GetKey ()] = NewFramebuffer;
        return NewHandle;
        }

error:
    if ( NewFramebuffer.DepthTexture )
        DeleteTexture ( NewFramebuffer.DepthTexture );
    for ( size_t cont = 0; cont < NewFramebuffer.ColorTextures.size(); ++cont )
        DeleteTexture ( NewFramebuffer.ColorTextures[cont] );
    glDeleteFramebuffers ( 1, &NewFramebuffer.OpenGLID );
    return FramebufferHandle::Invalid;
    }

bool DeleteFramebuffer ( const FramebufferHandle Handle )
    {
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];

	if ( FramebufferInformation->DepthTexture )
		DeleteTexture ( FramebufferInformation->DepthTexture );
	for ( size_t cont = 0; cont < FramebufferInformation->ColorTextures.size (); ++cont )
		DeleteTexture ( FramebufferInformation->ColorTextures[cont] );

	glDeleteFramebuffers ( 1, &FramebufferInformation->OpenGLID );
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

void ClearFramebuffer ( const FramebufferHandle &Handle, const bool ShouldClearColorBuffer, const glm::vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask )
	{
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];
	int BitMask = 0;
	if ( ( ( FramebufferInformation->OpenGLID == 0 ) || ( FramebufferInformation->ColorTextures.size () ) ) && ( ShouldClearColorBuffer ) )
		{
		BitMask |= GL_COLOR_BUFFER_BIT;
		glClearColor ( Color.r, Color.g, Color.b, Color.a );
		}
	if ( ( ( FramebufferInformation->OpenGLID == 0 ) || ( FramebufferInformation->DepthTexture ) ) && ( ShouldClearDepthBuffer ) )
		{
		BitMask |= GL_DEPTH_BUFFER_BIT;
		glClearDepth ( DepthClearValue );
		}
	if ( ( ( FramebufferInformation->OpenGLID == 0 ) || ( FramebufferInformation->StencilTexture ) ) && ( ShouldClearStencilBuffer ) )
		{
		BitMask |= GL_STENCIL_BUFFER_BIT;
		glStencilMask ( StencilMask );
		glClearStencil ( StencilClearValue );
		}
	if ( BitMask )
		{
		if ( CurrentBoundFramebuffer != Handle )
			{
			glBindFramebuffer ( GL_FRAMEBUFFER, FramebufferInformation->OpenGLID );
			CurrentBoundFramebuffer = Handle;
			}

		StateCache::SetDefaultViewportSize ( FramebufferInformation->Dimensions );
		StateCache::ConfigureScissor ( ScissorSettings () );
		StateCache::ConfigureViewport ( ViewportSettings () );
		StateCache::ConfigureStencil ( StencilBufferSettings () );
		glClear ( BitMask );
		}
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
