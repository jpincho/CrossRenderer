#include "OpenGLDSAFramebuffer.h"
#include "OpenGLDSATexture.h"
#include "OpenGLTexture.h"
#include "OpenGLInternals.h"
#include <Logger/Logger.h>

namespace CrossRenderer
{
namespace OpenGL
{
FramebufferHandle DSACreateFramebuffer ( const FramebufferDescriptor CreationParameters )
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
        NewFramebuffer.DepthTexture = DSACreateTexture ( NewTextureDescriptor );
        if ( !NewFramebuffer.DepthTexture )
            return FramebufferHandle::invalid;
        }
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.Format = CreationParameters.ColorAttachmentFormat;
        NewTextureDescriptor.Type = TextureType::Texture2D;
        TextureHandle NewTexture = DSACreateTexture ( NewTextureDescriptor );
        if ( !NewTexture )
            goto error;
        NewFramebuffer.ColorTextures.push_back ( NewTexture );
        }

    glCreateFramebuffers ( 1, &NewFramebuffer.OpenGLID );
    if  ( CreationParameters.DepthEnabled )
        glNamedFramebufferTexture ( NewFramebuffer.OpenGLID, GL_DEPTH_ATTACHMENT, Textures[NewFramebuffer.DepthTexture].OpenGLID, 0 );
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        glNamedFramebufferTexture ( NewFramebuffer.OpenGLID, GL_COLOR_ATTACHMENT0 + cont, Textures[NewFramebuffer.ColorTextures[cont]].OpenGLID, 0 );

    if ( CheckError() == false )
        goto error;

    FramebufferStatus = glCheckNamedFramebufferStatus ( NewFramebuffer.OpenGLID, GL_FRAMEBUFFER );

    if ( FramebufferStatus != GL_FRAMEBUFFER_COMPLETE )
        {
        LOG_ERROR ( "Unable to create framebuffer. %s", StringifyOpenGL ( FramebufferStatus ) );
        goto error;
        }

        {
        FramebufferHandle NewHandle = Framebuffers.GetNewHandle();
        Framebuffers[NewHandle] = NewFramebuffer;
        return NewHandle;
        }

error:
    if ( NewFramebuffer.DepthTexture )
        DeleteTexture ( NewFramebuffer.DepthTexture );
    for ( size_t cont = 0; cont < NewFramebuffer.ColorTextures.size(); ++cont )
        DeleteTexture ( NewFramebuffer.ColorTextures[cont] );
    glDeleteFramebuffers ( 1, &NewFramebuffer.OpenGLID );
    return FramebufferHandle::invalid;
    }
}
}
