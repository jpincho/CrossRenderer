#include "OpenGLTexture.h"
#include "OpenGLInternals.h"

namespace CrossRenderer
{
namespace OpenGL
{
TextureHandle CreateTexture ( const TextureDescriptor CreationParameters )
    {
    TextureInfo NewTexture;
    NewTexture.Dimensions = CreationParameters.Dimensions;
    NewTexture.Format = CreationParameters.Format;
    NewTexture.Type = CreationParameters.Type;
    NewTexture.GLSWrap = Translate ( CreationParameters.WrapSettings.Horizontal );
    NewTexture.GLTWrap = Translate ( CreationParameters.WrapSettings.Vertical );
    NewTexture.GLMinFilter = Translate ( CreationParameters.FilterSettings.MinFilter );
    NewTexture.GLMagFilter = Translate ( CreationParameters.FilterSettings.MagFilter );
    GLenum OriginDataType;
    GLint InternalFormat;
    switch ( NewTexture.Format )
        {
        case PixelFormat::Alpha8:
            OriginDataType = GL_UNSIGNED_BYTE;
            InternalFormat = GL_ALPHA;
            break;
        case PixelFormat::RedGreenBlue888:
        case PixelFormat::BlueGreenRed888:
            OriginDataType = GL_UNSIGNED_BYTE;
            InternalFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlue565:
        case PixelFormat::RedGreenBlueAlpha8888:
        case PixelFormat::BlueGreenRedAlpha8888:
        case PixelFormat::AlphaRedGreenBlue8888:
        case PixelFormat::AlphaBlueGreenRed8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            InternalFormat = GL_RGBA;
            break;
        // Texture format only
        case PixelFormat::DepthComponent:
            OriginDataType = GL_FLOAT;
            InternalFormat = GL_DEPTH_COMPONENT;
            break;
        default:
            return TextureHandle::invalid;
        }

    glGenTextures ( 1, &NewTexture.OpenGLID );
    if ( CheckError() == false )
        {
        return TextureHandle::invalid;
        }
    switch ( NewTexture.Type )
        {
        case TextureType::Texture2D:
            {
            glBindTexture ( GL_TEXTURE_2D, NewTexture.OpenGLID );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, NewTexture.GLSWrap );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, NewTexture.GLTWrap );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NewTexture.GLMinFilter );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, NewTexture.GLMagFilter );
            glTexImage2D ( GL_TEXTURE_2D,
                           0,
                           InternalFormat,
                           ( GLint ) NewTexture.Dimensions.x, ( GLint ) NewTexture.Dimensions.y,
                           0,
                           InternalFormat,
                           OriginDataType,
                           nullptr );
            break;
            }
        default:
            return TextureHandle::invalid;
        }

    if ( CheckError() == false )
        {
        glDeleteTextures ( 1, &NewTexture.OpenGLID );
        return TextureHandle::invalid;
        }

    TextureHandle NewHandle = Textures.GetNewHandle();
    Textures[NewHandle] = NewTexture;
    return NewHandle;
    }

TextureHandle DSACreateTexture ( const TextureDescriptor CreationParameters )
    {
    TextureInfo NewTexture;
    NewTexture.Dimensions = CreationParameters.Dimensions;
    NewTexture.Format = CreationParameters.Format;
    NewTexture.Type = CreationParameters.Type;
    NewTexture.GLSWrap = Translate ( CreationParameters.WrapSettings.Horizontal );
    NewTexture.GLTWrap = Translate ( CreationParameters.WrapSettings.Vertical );
    NewTexture.GLMinFilter = Translate ( CreationParameters.FilterSettings.MinFilter );
    NewTexture.GLMagFilter = Translate ( CreationParameters.FilterSettings.MagFilter );
    GLint SizedInternalFormat;
    switch ( NewTexture.Format )
        {
        case PixelFormat::Alpha8:
            SizedInternalFormat = GL_ALPHA8;
            break;
        case PixelFormat::RedGreenBlue888:
        case PixelFormat::BlueGreenRed888:
            SizedInternalFormat = GL_RGB8;
            break;
        case PixelFormat::RedGreenBlue565:
        case PixelFormat::RedGreenBlueAlpha8888:
        case PixelFormat::BlueGreenRedAlpha8888:
        case PixelFormat::AlphaRedGreenBlue8888:
        case PixelFormat::AlphaBlueGreenRed8888:
            SizedInternalFormat = GL_RGBA8;
            break;
        // Texture format only
        case PixelFormat::DepthComponent:
            SizedInternalFormat = GL_DEPTH_COMPONENT24;
            break;
        default:
            return TextureHandle::invalid;
        }

    switch ( NewTexture.Type )
        {
        case TextureType::Texture2D:
            {
            glCreateTextures ( GL_TEXTURE_2D, 1, &NewTexture.OpenGLID );
            if ( CheckError() == false )
                {
                return TextureHandle::invalid;
                }
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_WRAP_S, NewTexture.GLSWrap );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_WRAP_T, NewTexture.GLTWrap );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_MIN_FILTER, NewTexture.GLMinFilter );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_MAG_FILTER, NewTexture.GLMagFilter );
            glTextureStorage2D ( NewTexture.OpenGLID, 1, SizedInternalFormat, NewTexture.Dimensions.x, NewTexture.Dimensions.y );
            break;
            }
        default:
            return TextureHandle::invalid;
        }

    if ( CheckError() == false )
        {
        glDeleteTextures ( 1, &NewTexture.OpenGLID );
        return TextureHandle::invalid;
        }

    TextureHandle NewHandle = Textures.GetNewHandle();
    Textures[NewHandle] = NewTexture;
    return NewHandle;
    }

bool DeleteTexture ( const TextureHandle Handle )
    {
    TextureInfo *TextureInformation = &Textures[Handle];

    glDeleteTextures ( 1, &TextureInformation->OpenGLID );
    Textures.ReleaseHandle ( Handle );
    return true;
    }

bool Load2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data, const unsigned Pitch )
    {
    TextureInfo *TextureInformation = &Textures[Handle];

    uint8_t alignment = 1;
    uint8_t alignment_mask;

    for ( alignment_mask = 1; alignment_mask <= 8; alignment_mask *= 2 )
        {
        if ( Pitch % alignment_mask == 0 )
            alignment = alignment_mask;
        }
    glPixelStorei ( GL_UNPACK_ALIGNMENT, alignment );
    // OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL.
    // OriginDataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
    // DestinationInternalFormat - GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_RG, GL_RGB, GL_RGBA
    // (Sized) - GL_R8, GL_R8_SNORM, GL_R16, GL_R16_SNORM, GL_RG8, GL_RG8_SNORM, GL_RG16, GL_RG16_SNORM, GL_R3_G3_B2, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB8_SNORM, GL_RGB10, GL_RGB12, GL_RGB16_SNORM, GL_RGBA2, GL_RGBA4, GL_RGB5_A1
    // GL_RGBA8, GL_RGBA8_SNORM, GL_RGB10_A2, GL_RGB10_A2UI, GL_RGBA12, GL_RGBA16, GL_SRGB8, GL_SRGB8_ALPHA8, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F, GL_R11F_G11F_B10F, GL_RGB9_E5
    // GL_R8I, GL_R8UI, GL_R16I, GL_R16UI, GL_R32I, GL_R32UI, GL_RG8I, GL_RG8UI, GL_RG16I, GL_RG16UI, GL_RG32I, GL_RG32UI, GL_RGB8I, GL_RGB8UI, GL_RGB16I, GL_RGB16UI, GL_RGB32I, GL_RGB32UI, GL_RGBA8I, GL_RGBA8UI, GL_RGBA16I, GL_RGBA16UI, GL_RGBA32I, GL_RGBA32UI
    // (Compressed) - GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_COMPRESSED_SRGB, GL_COMPRESSED_SRGB_ALPHA, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_RG_RGTC2
    // GL_COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    GLenum OriginPixelFormat, OriginDataType;
    GLint DestinationInternalFormat;
    switch ( SourcePixelFormat )
        {
        case PixelFormat::Alpha8:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_ALPHA;
            DestinationInternalFormat = GL_ALPHA;
            break;
        case PixelFormat::RedGreenBlue888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            DestinationInternalFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlue565:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            DestinationInternalFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlueAlpha8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGBA;
            DestinationInternalFormat = GL_RGBA;
            break;
        case PixelFormat::BlueGreenRed888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_BGR;
            DestinationInternalFormat = GL_RGB;
            break;
        case PixelFormat::BlueGreenRedAlpha8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_BGRA;
            DestinationInternalFormat = GL_RGBA;
            break;
        case PixelFormat::AlphaRedGreenBlue8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            DestinationInternalFormat = GL_RGBA;
            break;
        case PixelFormat::AlphaBlueGreenRed8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            DestinationInternalFormat = GL_RGBA;
            break;
        case PixelFormat::DepthComponent:
            OriginDataType = GL_DEPTH_COMPONENT;
            OriginPixelFormat = GL_FLOAT;
            DestinationInternalFormat = GL_DEPTH_COMPONENT;
            break;
        default:
            return false;
        }

    switch ( TextureInformation->Type )
        {
        case TextureType::Texture2D:
            {
            glBindTexture ( GL_TEXTURE_2D, TextureInformation->OpenGLID );
            glTexImage2D ( GL_TEXTURE_2D,
                           0,
                           DestinationInternalFormat,
                           TextureInformation->Dimensions.x, TextureInformation->Dimensions.y,
                           0,
                           OriginPixelFormat,
                           OriginDataType,
                           Data );
            break;
            }
        default:
            return false;
        }

    if ( CheckError() == false )
        return false;
    glBindTexture ( GL_TEXTURE_2D, 0 );
    return true;
    }

bool DSALoad2DTextureData ( const TextureHandle Handle, const PixelFormat, const void *Data, const unsigned )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    return DSAUpdateTextureRegion ( Handle, glm::uvec2 ( 0, 0 ), TextureInformation->Dimensions, Data );
    }

bool UpdateTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const void *Data )
    {
    TextureInfo *TextureInformation = &Textures[Handle];

    if ( ( LowerLeft.x + RegionDimensions.x > TextureInformation->Dimensions.x ) ||
            ( LowerLeft.y + RegionDimensions.y > TextureInformation->Dimensions.y ) )
        return false;

    uint8_t alignment = 1;
    uint32_t alignment_mask;
    for ( alignment_mask = 1; alignment_mask <= 8; alignment_mask *= 2 )
        {
        if ( ( RegionDimensions.x % alignment_mask == 0 ) && ( reinterpret_cast<long long> ( Data ) % alignment_mask == 0 ) )
            alignment = alignment_mask & 0xFF;
        }
    glPixelStorei ( GL_UNPACK_ALIGNMENT, alignment );

    // OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
    // OriginDataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
    GLenum OriginPixelFormat, OriginDataType;
    switch ( TextureInformation->Format )
        {
        case PixelFormat::Alpha8:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_ALPHA;
            break;
        case PixelFormat::RedGreenBlue888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlue565:
            OriginDataType = GL_UNSIGNED_SHORT_5_6_5;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlueAlpha8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGBA;
            break;
        case PixelFormat::BlueGreenRed888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_BGR;
            break;
        case PixelFormat::BlueGreenRedAlpha8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_BGRA;
            break;
        case PixelFormat::AlphaRedGreenBlue8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::AlphaBlueGreenRed8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::DepthComponent:
            OriginDataType = GL_DEPTH_COMPONENT;
            OriginPixelFormat = GL_FLOAT;
            break;
        default:
            return false;
        }
    switch ( TextureInformation->Type )
        {
        case TextureType::Texture2D:
            {
            glBindTexture ( GL_TEXTURE_2D, TextureInformation->OpenGLID );
            glTexSubImage2D ( GL_TEXTURE_2D,
                              0,
                              LowerLeft.x, LowerLeft.y,
                              ( int ) RegionDimensions.x, ( int ) RegionDimensions.y,
                              OriginPixelFormat,
                              OriginDataType,
                              Data );
            break;
            }
        default:
            return false;
        }
    return true;
    }

bool DSAUpdateTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const void *Data )
    {
    TextureInfo *TextureInformation = &Textures[Handle];

    if ( ( LowerLeft.x + RegionDimensions.x > TextureInformation->Dimensions.x ) ||
            ( LowerLeft.y + RegionDimensions.y > TextureInformation->Dimensions.y ) )
        return false;

    uint8_t alignment = 1;
    uint32_t alignment_mask;
    for ( alignment_mask = 1; alignment_mask <= 8; alignment_mask *= 2 )
        {
        if ( ( RegionDimensions.x % alignment_mask == 0 ) && ( reinterpret_cast<long long> ( Data ) % alignment_mask == 0 ) )
            alignment = alignment_mask & 0xFF;
        }
    glPixelStorei ( GL_UNPACK_ALIGNMENT, alignment );

    // OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
    // OriginDataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
    GLenum OriginPixelFormat, OriginDataType;
    switch ( TextureInformation->Format )
        {
        case PixelFormat::Alpha8:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_ALPHA;
            break;
        case PixelFormat::RedGreenBlue888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlue565:
            OriginDataType = GL_UNSIGNED_SHORT_5_6_5;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::RedGreenBlueAlpha8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGBA;
            break;
        case PixelFormat::BlueGreenRed888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_BGR;
            break;
        case PixelFormat::BlueGreenRedAlpha8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_BGRA;
            break;
        case PixelFormat::AlphaRedGreenBlue8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::AlphaBlueGreenRed8888:
            OriginDataType = GL_UNSIGNED_BYTE;
            OriginPixelFormat = GL_RGB;
            break;
        case PixelFormat::DepthComponent:
            OriginDataType = GL_DEPTH_COMPONENT;
            OriginPixelFormat = GL_FLOAT;
            break;
        default:
            return false;
        }
    switch ( TextureInformation->Type )
        {
        case TextureType::Texture2D:
            {
            glTextureSubImage2D ( TextureInformation->OpenGLID,
                                  0,
                                  LowerLeft.x, LowerLeft.y,
                                  ( int ) RegionDimensions.x, ( int ) RegionDimensions.y,
                                  OriginPixelFormat,
                                  OriginDataType,
                                  Data );
            break;
            }
        default:
            return false;
        }

    return true;
    }

glm::uvec2 GetTextureDimensions ( const TextureHandle Handle )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    return TextureInformation->Dimensions;
    }
}
}
