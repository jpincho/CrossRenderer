#include "OpenGLDSAShaderBuffer.h"
#include "OpenGLInternals.h"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderBufferHandle DSACreateShaderBuffer ( const ShaderBufferDescriptor CreationParameters )
    {
    ShaderBufferInfo NewBufferInformation;
    NewBufferInformation = CreationParameters;
    NewBufferInformation.GLAccessType = Translate ( CreationParameters.AccessType );
    NewBufferInformation.GLBufferType = Translate ( CreationParameters.BufferType );
    NewBufferInformation.MappedPointer = nullptr;

    glCreateBuffers ( 1, &NewBufferInformation.OpenGLID );
    glNamedBufferData ( NewBufferInformation.OpenGLID, NewBufferInformation.DataSize, CreationParameters.Data, NewBufferInformation.GLAccessType );
    if ( CheckError() == false )
        {
        glDeleteBuffers ( 1, &NewBufferInformation.OpenGLID );
        return ShaderBufferHandle::invalid;
        }

    ShaderBufferHandle NewHandle = ShaderBuffers.GetNewHandle();
    ShaderBuffers[NewHandle] = NewBufferInformation;
    return NewHandle;
    }

bool DSAChangeShaderBufferContents ( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters )
    {
    ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];

    if ( ShaderBufferInformation->MappedPointer )
        DSAUnmapShaderBuffer ( Handle );

    if ( CreationParameters.DataSize <= ShaderBufferInformation->DataSize )
        glNamedBufferSubData ( ShaderBufferInformation->OpenGLID, 0, CreationParameters.DataSize, CreationParameters.Data );
    else
        glNamedBufferData ( ShaderBufferInformation->OpenGLID, CreationParameters.DataSize, CreationParameters.Data, ShaderBufferInformation->GLAccessType );
    ShaderBufferInformation->DataSize = CreationParameters.DataSize;
    return CheckError();
    }

void *DSAMapShaderBuffer ( const ShaderBufferHandle Handle, const ShaderBufferAccessType AccessType )
    {
    ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];
    GLenum GLAccessType = Translate ( AccessType );

    if ( ( ShaderBufferInformation->MappedPointer ) && ( ShaderBufferInformation->GLMappedAccessType == GLAccessType ) )
        return ShaderBufferInformation->MappedPointer;

    ShaderBufferInformation->MappedPointer = glMapNamedBuffer ( ShaderBufferInformation->OpenGLID, GLAccessType );
    ShaderBufferInformation->GLMappedAccessType = GLAccessType;
    if ( CheckError() == false )
        return nullptr;

    return ShaderBufferInformation->MappedPointer;
    }

bool DSAUnmapShaderBuffer ( const ShaderBufferHandle Handle )
    {
    ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];

    if ( ShaderBufferInformation->MappedPointer == nullptr )
        return true;

    glUnmapNamedBuffer ( ShaderBufferInformation->OpenGLID );

    ShaderBufferInformation->MappedPointer = nullptr;
    return CheckError();
    }
}
}
