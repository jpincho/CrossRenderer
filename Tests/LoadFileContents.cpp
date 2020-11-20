#include "LoadFileContents.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

bool LoadFileContents ( const std::string &Path, std::vector <uint8_t> &Contents )
    {
    FILE *FileHandle = fopen ( Path.c_str(), "rb" );

    if ( !FileHandle )
        return false;
    fseek ( FileHandle, 0, SEEK_END );
    size_t FileSize = ftell ( FileHandle );
    fseek ( FileHandle, 0, SEEK_SET );
    Contents.resize ( FileSize );
    size_t TotalBytesRead = 0;
    while ( TotalBytesRead < FileSize )
        {
        size_t BytesRead = fread ( Contents.data() + TotalBytesRead, 1, std::min <size_t> ( 16384, FileSize - TotalBytesRead ), FileHandle );
        if ( BytesRead <= 0 )
            {
            fclose ( FileHandle );
            return false;
            }
        TotalBytesRead += BytesRead;
        }
    fclose ( FileHandle );
    return true;
    }

bool LoadTextureFile ( const std::string &Path, CrossRenderer::TextureHandle &TextureObject )
    {
    glm::ivec2 ImageSize;
    int Channels;
    CrossRenderer::PixelFormat ImageFormat;
    stbi_uc *Image = stbi_load ( Path.c_str(), &ImageSize.x, &ImageSize.y, &Channels, 0 );
    if ( !Image )
        return false;

    switch ( Channels )
        {
        case 3:
            ImageFormat = CrossRenderer::PixelFormat::RedGreenBlue888;
            break;
        case 4:
            ImageFormat = CrossRenderer::PixelFormat::RedGreenBlueAlpha8888;
            break;
        default:
            stbi_image_free ( Image );
            return false;
        }

    CrossRenderer::TextureDescriptor TextureDescriptor;
    TextureDescriptor.Dimensions = glm::uvec2 ( ImageSize.x, ImageSize.y );
    TextureDescriptor.TextureFormat = ImageFormat;
    TextureObject = CrossRenderer::Create2DTexture ( TextureDescriptor );
    if ( !TextureObject )
        {
        stbi_image_free ( Image );
        return false;
        }
    if ( CrossRenderer::Load2DTextureData ( TextureObject, ImageFormat, Image, 0 ) == false )
        {
        stbi_image_free ( Image );
        return false;
        }

    stbi_image_free ( Image );
    return true;
    }
