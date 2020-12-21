#include "OpenGLFramebuffer.h"
#include "OpenGLInternals.h"
#include "OpenGLTexture.h"
#include <Logger/Logger.h>

namespace CrossRenderer
{
namespace OpenGL
{
FramebufferHandle CreateFramebuffer ( const FramebufferDescriptor CreationParameters )
    {
    FramebufferInfo NewFramebuffer;
    GLenum FramebufferStatus;

    NewFramebuffer.Dimensions = CreationParameters.Dimensions;
    NewFramebuffer.ClearColor = CreationParameters.DefaultClearColor;
    NewFramebuffer.ClearDepth = CreationParameters.DefaultClearDepth;

    if ( CreationParameters.DepthEnabled )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.TextureFormat = CreationParameters.DepthFormat;
        NewFramebuffer.DepthTexture = Create2DTexture ( NewTextureDescriptor );
        if ( !NewFramebuffer.DepthTexture )
            return FramebufferHandle::invalid;
        }
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.TextureFormat = CreationParameters.ColorAttachmentFormat;
        TextureHandle NewTexture = Create2DTexture ( NewTextureDescriptor );
        if ( !NewTexture )
            goto error;
        NewFramebuffer.ColorTextures.push_back ( NewTexture );
        }

    glGenFramebuffers ( 1, &NewFramebuffer.OpenGLID );
    glBindFramebuffer ( GL_FRAMEBUFFER, NewFramebuffer.OpenGLID );
    if  ( CreationParameters.DepthEnabled )
        glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Textures[NewFramebuffer.DepthTexture].OpenGLID, 0 );
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + cont, GL_TEXTURE_2D, Textures[NewFramebuffer.ColorTextures[cont]].OpenGLID, 0 );

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

FramebufferHandle DSACreateFramebuffer ( const FramebufferDescriptor CreationParameters )
    {
    FramebufferInfo NewFramebuffer;
    GLenum FramebufferStatus;

    NewFramebuffer.Dimensions = CreationParameters.Dimensions;
    NewFramebuffer.ClearDepth = CreationParameters.DefaultClearDepth;
    NewFramebuffer.ClearColor = CreationParameters.DefaultClearColor;

    if ( CreationParameters.DepthEnabled )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.TextureFormat = CreationParameters.DepthFormat;
        NewFramebuffer.DepthTexture = Create2DTexture ( NewTextureDescriptor );
        if ( !NewFramebuffer.DepthTexture )
            return FramebufferHandle::invalid;
        }
    for ( unsigned cont = 0; cont < CreationParameters.ColorAttachments; ++cont )
        {
        TextureDescriptor NewTextureDescriptor;
        NewTextureDescriptor.Dimensions = NewFramebuffer.Dimensions;
        NewTextureDescriptor.TextureFormat = CreationParameters.ColorAttachmentFormat;
        TextureHandle NewTexture = Create2DTexture ( NewTextureDescriptor );
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

bool DeleteFramebuffer ( const FramebufferHandle Handle )
    {
    FramebufferInfo *info = &Framebuffers[Handle];

    if ( info->DepthTexture )
        DeleteTexture ( info->DepthTexture );
    for ( size_t cont = 0; cont < info->ColorTextures.size(); ++cont )
        DeleteTexture ( info->ColorTextures[cont] );

    glDeleteFramebuffers ( 1, &info->OpenGLID );
    Framebuffers.ReleaseHandle ( Handle );
    return true;
    }

bool BindFramebuffer ( const FramebufferHandle Handle )
    {
    FramebufferInfo *info = &Framebuffers[Handle];

    glBindFramebuffer ( GL_FRAMEBUFFER, info->OpenGLID );
    return CheckError();
    }

bool UnbindFramebuffer ( void )
    {
    glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
    return CheckError();
    }

glm::uvec2 GetFramebufferSize ( const FramebufferHandle Handle )
    {
    FramebufferInfo *info = &Framebuffers[Handle];

    return info->Dimensions;
    }

TextureHandle GetColorBufferFromFramebuffer ( const FramebufferHandle Handle, const size_t Index )
    {
    FramebufferInfo *info = &Framebuffers[Handle];

    if ( Index > info->ColorTextures.size() )
        return TextureHandle::invalid;
    return info->ColorTextures[Index];
    }

TextureHandle GetDepthBufferFromFramebuffer ( const FramebufferHandle Handle )
    {
    FramebufferInfo *info = &Framebuffers[Handle];

    return info->DepthTexture;
    }
}
}
