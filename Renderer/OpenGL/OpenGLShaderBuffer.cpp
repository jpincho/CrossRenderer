#include "OpenGLShaderBuffer.hpp"
#include "OpenGLInternals.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderBufferHandle CreateShaderBuffer ( const ShaderBufferDescriptor CreationParameters )
	{
	ShaderBufferInfo NewBufferInformation;
	NewBufferInformation = CreationParameters;
	NewBufferInformation.GLAccessType = Translate ( CreationParameters.AccessType );
	NewBufferInformation.GLBufferType = Translate ( CreationParameters.BufferType );
	NewBufferInformation.MappedPointer = nullptr;

	if ( OpenGLInformation.DirectStateAccessEnabled )
		{
		glCreateBuffers ( 1, &NewBufferInformation.OpenGLID );
		if ( CheckError () == false )
			return ShaderBufferHandle::Invalid;
		glNamedBufferData ( NewBufferInformation.OpenGLID, NewBufferInformation.DataSize, CreationParameters.Data, NewBufferInformation.GLAccessType );
		if ( CheckError () == false )
			{
			glDeleteBuffers ( 1, &NewBufferInformation.OpenGLID );
			return ShaderBufferHandle::Invalid;
			}
		}
	else
		{
		glGenBuffers ( 1, &NewBufferInformation.OpenGLID );
		if ( CheckError () == false )
			return ShaderBufferHandle::Invalid;

		glBindBuffer ( NewBufferInformation.GLBufferType, NewBufferInformation.OpenGLID );
		glBufferData ( NewBufferInformation.GLBufferType, NewBufferInformation.DataSize, CreationParameters.Data, NewBufferInformation.GLAccessType );
		if ( CheckError () == false )
			{
			glDeleteBuffers ( 1, &NewBufferInformation.OpenGLID );
			return ShaderBufferHandle::Invalid;
			}
		}

	ShaderBufferHandle NewHandle ( ShaderBuffers.GetFreeIndex () );
	ShaderBuffers[NewHandle.GetKey ()] = NewBufferInformation;
	return NewHandle;
	}

bool DeleteShaderBuffer ( const ShaderBufferHandle Handle )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];
	glDeleteBuffers ( 1, &ShaderBufferInformation->OpenGLID );
	ShaderBuffers.ReleaseIndex ( Handle.GetKey () );
	return true;
	}

bool ChangeShaderBufferContents ( const ShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];

	if ( DataSize > ShaderBufferInformation->DataSize )
		return false;

	if ( OpenGLInformation.DirectStateAccessEnabled )
		{
		glNamedBufferSubData ( ShaderBufferInformation->OpenGLID, Offset, DataSize, Data );
		}
	else
		{
		glBindBuffer ( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
		glBufferSubData ( ShaderBufferInformation->GLBufferType, Offset, DataSize, Data );

		glBindBuffer ( ShaderBufferInformation->GLBufferType, 0 ); // Unbind it
		}
	return CheckError ();
	}

void *MapShaderBuffer ( const ShaderBufferHandle Handle, const ShaderBufferMapAccessType AccessType )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];
	GLenum GLAccessType = Translate ( AccessType );

	if ( ( ShaderBufferInformation->MappedPointer ) && ( ShaderBufferInformation->GLMappedAccessType == GLAccessType ) )
		return ShaderBufferInformation->MappedPointer;

	if ( OpenGLInformation.DirectStateAccessEnabled )
		{
		ShaderBufferInformation->MappedPointer = glMapNamedBuffer ( ShaderBufferInformation->OpenGLID, GLAccessType );
		ShaderBufferInformation->GLMappedAccessType = GLAccessType;
		if ( CheckError () == false )
			return nullptr;
		}
	else
		{
		glBindBuffer ( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
		if ( CheckError () == false )
			return nullptr;
		ShaderBufferInformation->MappedPointer = glMapBuffer ( ShaderBufferInformation->GLBufferType, GLAccessType );
		ShaderBufferInformation->GLMappedAccessType = GLAccessType;
		if ( CheckError () == false )
			return nullptr;
		}
	return ShaderBufferInformation->MappedPointer;
	}

bool UnmapShaderBuffer ( const ShaderBufferHandle Handle )
	{
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];

	if ( ShaderBufferInformation->MappedPointer == nullptr )
		return true;

	if ( OpenGLInformation.DirectStateAccessEnabled )
		{
		glUnmapNamedBuffer ( ShaderBufferInformation->OpenGLID );
		}
	else
		{
		glBindBuffer ( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
		glUnmapBuffer ( ShaderBufferInformation->GLBufferType );
		}
	ShaderBufferInformation->MappedPointer = nullptr;
	return CheckError ();
	}

const ShaderBufferDescriptor GetShaderBufferDescriptor ( const ShaderBufferHandle Handle )
	{
	return ShaderBuffers[Handle.GetKey ()];
	}
}
}
