#include "OpenGLDSATexture.h"
#include "OpenGLInternals.h"

namespace CrossRenderer
{
namespace OpenGL
{
static struct
    {
    GLint DataType;
    GLint Format;
    GLint SizedFormat;
    } PixelFormatData[] =
    {
        {GL_UNSIGNED_BYTE, GL_RGB, GL_RGB8},// RedGreenBlue888
        {GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8},// RedGreenBlueAlpha8888
        {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24}// DepthComponent
    };

TextureHandle DSACreateTexture ( const TextureDescriptor CreationParameters )
    {
    TextureInfo NewTexture;
    NewTexture = CreationParameters;
    NewTexture.GLSWrap = Translate ( CreationParameters.WrapSettings.Horizontal );
    NewTexture.GLTWrap = Translate ( CreationParameters.WrapSettings.Vertical );
    NewTexture.GLMinFilter = Translate ( CreationParameters.FilterSettings.MinFilter );
    NewTexture.GLMagFilter = Translate ( CreationParameters.FilterSettings.MagFilter );

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
            glTextureStorage2D ( NewTexture.OpenGLID,
                                 1,
                                 PixelFormatData[ ( int ) NewTexture.Format].SizedFormat,
                                 NewTexture.Dimensions.x, NewTexture.Dimensions.y );
            break;
            }
        case TextureType::TextureCubeMap:
            {
            glCreateTextures ( GL_TEXTURE_CUBE_MAP, 1, &NewTexture.OpenGLID );
            if ( CheckError() == false )
                {
                return TextureHandle::invalid;
                }
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_WRAP_S, NewTexture.GLSWrap );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_WRAP_T, NewTexture.GLTWrap );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_MIN_FILTER, NewTexture.GLMinFilter );
            glTextureParameteri ( NewTexture.OpenGLID, GL_TEXTURE_MAG_FILTER, NewTexture.GLMagFilter );
            glTextureStorage2D ( NewTexture.OpenGLID,
                                 1,
                                 PixelFormatData[ ( int ) NewTexture.Format].SizedFormat,
                                 NewTexture.Dimensions.x, NewTexture.Dimensions.y );
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

bool DSALoad2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    return DSAUpdate2DTextureRegion ( Handle, glm::uvec2 ( 0, 0 ), TextureInformation->Dimensions, SourcePixelFormat, Data );
    }

bool DSAUpdate2DTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data )
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
    glTextureSubImage2D ( TextureInformation->OpenGLID,
                          0,
                          LowerLeft.x, LowerLeft.y,
                          ( int ) RegionDimensions.x, ( int ) RegionDimensions.y,
                          PixelFormatData[ ( int ) SourcePixelFormat].Format,
                          PixelFormatData[ ( int ) SourcePixelFormat].DataType,
                          Data );
    if ( TextureInformation->Mipmapped )
        glGenerateTextureMipmap ( TextureInformation->OpenGLID );
    return CheckError();
    }

bool DSALoadCubeMapTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    for ( unsigned Face = 0; Face < 6; ++Face )
        {
        glTextureSubImage3D ( TextureInformation->OpenGLID,
                              0,
                              0, 0, Face,
                              TextureInformation->Dimensions.x, TextureInformation->Dimensions.y, 1,
                              PixelFormatData[ ( int ) SourcePixelFormat].Format,
                              PixelFormatData[ ( int ) SourcePixelFormat].DataType,
                              Data[Face] );
        }
    if ( TextureInformation->Mipmapped )
        glGenerateTextureMipmap ( TextureInformation->OpenGLID );
    return CheckError();
    }
}
}
