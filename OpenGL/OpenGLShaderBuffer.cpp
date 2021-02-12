#include "OpenGLShaderBuffer.h"
#include "OpenGLInternals.h"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderBufferHandle CreateShaderBuffer( const ShaderBufferDescriptor CreationParameters )
	{
	ShaderBufferInfo NewBufferInformation;
	NewBufferInformation.GLAccessType = Translate( CreationParameters.AccessType );
	NewBufferInformation.GLBufferType = Translate( CreationParameters.BufferType );
	NewBufferInformation.MappedPointer = nullptr;
	NewBufferInformation.DataSize = CreationParameters.DataSize;
	if ( BufferStorageAvailable )
		NewBufferInformation.Flags = CreationParameters.Flags;

	glGenBuffers( 1, &NewBufferInformation.OpenGLID );
	if ( CheckError() == false )
		return ShaderBufferHandle::invalid;

	if ( BufferStorageAvailable )
		{
		int Flags = 0;
		Flags |= GL_DYNAMIC_STORAGE_BIT;
		if ( CreationParameters.Flags.Mappable )
			Flags |= GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
		glBindBuffer( NewBufferInformation.GLBufferType, NewBufferInformation.OpenGLID );
		glBufferStorage( NewBufferInformation.GLBufferType, NewBufferInformation.DataSize, CreationParameters.Data, Flags );
		}
	else
		{
		glBufferData( NewBufferInformation.GLBufferType, NewBufferInformation.DataSize, CreationParameters.Data, NewBufferInformation.GLAccessType );
		}
	if ( CheckError() == false )
		{
		glDeleteBuffers( 1, &NewBufferInformation.OpenGLID );
		return ShaderBufferHandle::invalid;
		}

	ShaderBufferHandle NewHandle = ShaderBuffers.GetNewHandle();
	ShaderBuffers[NewHandle] = NewBufferInformation;
	return NewHandle;
	}

bool DeleteShaderBuffer( const ShaderBufferHandle Handle )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];
	glDeleteBuffers( 1, &ShaderBufferInformation->OpenGLID );
	ShaderBuffers.ReleaseHandle( Handle );
	return true;
	}

bool ChangeShaderBufferContents( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];

	glBindBuffer( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
	if ( CreationParameters.DataSize <= ShaderBufferInformation->DataSize )
		glBufferSubData( ShaderBufferInformation->GLBufferType, 0, CreationParameters.DataSize, CreationParameters.Data );
	else
		glBufferData( ShaderBufferInformation->GLBufferType, CreationParameters.DataSize, CreationParameters.Data, ShaderBufferInformation->GLAccessType );
	glBindBuffer( ShaderBufferInformation->GLBufferType, 0 ); // Unbind it
	ShaderBufferInformation->DataSize = CreationParameters.DataSize;
	return CheckError();
	}

void *MapShaderBuffer( const ShaderBufferHandle Handle, const ShaderBufferAccessType AccessType )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];
	GLenum GLAccessType = Translate( AccessType );

	if ( ( ShaderBufferInformation->MappedPointer ) && ( ShaderBufferInformation->GLMappedAccessType == GLAccessType ) )
		return ShaderBufferInformation->MappedPointer;

	glBindBuffer( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
	ShaderBufferInformation->MappedPointer = glMapBuffer( ShaderBufferInformation->GLBufferType, GLAccessType );
	ShaderBufferInformation->GLMappedAccessType = GLAccessType;
	if ( CheckError() == false )
		return nullptr;

	return ShaderBufferInformation->MappedPointer;
	}

bool UnmapShaderBuffer( const ShaderBufferHandle Handle )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle];

	if ( ShaderBufferInformation->MappedPointer == nullptr )
		return true;

	glBindBuffer( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
	glUnmapBuffer( ShaderBufferInformation->GLBufferType );

	ShaderBufferInformation->MappedPointer = nullptr;
	return CheckError();
	}
}
}
