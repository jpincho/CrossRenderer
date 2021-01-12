#include "LoadFileContents.h"
#include <CrossRendererConfig.h>

bool LoadFileContents ( const std::string &Path, std::vector <uint8_t> &Contents )
    {
    FILE *FileHandle;
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
    if ( fopen_s ( &FileHandle, Path.c_str(), "rb" ) != 0 )
        return false;
#else
    FileHandle = fopen ( Path.c_str(), "rb" );
#endif
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
