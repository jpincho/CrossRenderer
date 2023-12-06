#include <CrossRendererConfig.hpp>
#include "Renderer.hpp"
#include "../Internal/Logger.hpp"
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT)
#include "OpenGL/OpenGLRenderer.hpp"
#endif

namespace CrossRenderer
{
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return (*name) ( __VA_ARGS__ ) = 0

#include "RendererDeclarations.hpp"

#undef DECLARE_INTERFACE_FUNCTION

void CreateNewRenderer ( const RendererBackend &Backend )
	{
	switch ( Backend )
		{
#if defined (CROSS_RENDERER_OPENGL_CORE_SUPPORT)
		case RendererBackend::OpenGLCore:
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
name = OpenGL::name

#include "RendererDeclarations.hpp"

#undef DECLARE_INTERFACE_FUNCTION
			break;
#endif
		default:
			LOG_ERROR ( "Renderer backend '%s' is not built", Stringify ( Backend ) );
			return;
		}
	}

bool SanityCheckRenderCommand ( const RenderCommand& Command )
	{
	ShaderInformation Info;
	bool Result = true;
	if ( !Command.Shader )
		{
		LOG_ERROR ( "Invalid shader handle" );
		return false;
		}
	GetShaderInformation ( Command.Shader, Info );

	// Find invalid uniform handles
	for ( auto &Iterator : Command.UniformValues )
		{
		if ( Iterator.UniformHandle == ShaderUniformHandle::Invalid )
			{
			LOG_ERROR ( "Invalid uniform handle" );
			Result = false;
			break;
			}
		}

	// Find invalid attribute handles
	for ( auto &Iterator : Command.ShaderBufferBindings )
		{
		if ( Iterator.AttributeHandle == ShaderAttributeHandle::Invalid )
			{
			LOG_ERROR ( "Invalid attribute handle" );
			Result = false;
			break;
			}
		}

	// Find invalid texture uniform handles
	for ( auto &Iterator : Command.TextureBindings )
		{
		if ( Iterator.UniformHandle == ShaderUniformHandle::Invalid )
			{
			LOG_ERROR ( "Invalid texture uniform handle" );
			Result = false;
			break;
			}
		}

	// Find duplicate uniform values
	auto FindUniformIndexFromHandle = [&Info] ( ShaderUniformHandle Handle )->int
		{
		for ( unsigned Iterator = 0; Iterator < Info.Uniforms.size(); ++Iterator )
			{
			if ( Info.Uniforms[Iterator].Handle == Handle )
				return ( int ) Iterator;
			}
		return -1;
		};
	for ( unsigned Iterator = 0; Iterator < Command.UniformValues.size(); ++Iterator )
		{
		int Index = FindUniformIndexFromHandle ( Command.UniformValues[Iterator].UniformHandle );
		if ( Index == -1 )
			{
			LOG_ERROR ( "Shader uniform handle does not belong to shader" );
			Result = false;
			continue;
			}
		for ( unsigned Iterator2 = Iterator + 1; Iterator2 < Command.UniformValues.size(); ++Iterator2 )
			{
			if ( Command.UniformValues[Iterator].UniformHandle == Command.UniformValues[Iterator2].UniformHandle )
				{
				LOG_ERROR ( "Duplicate value for uniform '%s'", Info.Uniforms[Index].Name.c_str() );
				Result = false;
				break;
				}
			}
		}

	// Find duplicate attribute bindings
	auto FindAttributeIndexFromHandle = [&Info] ( ShaderAttributeHandle Handle )->int
		{
		for ( unsigned Iterator = 0; Iterator < Info.Attributes.size(); ++Iterator )
			{
			if ( Info.Attributes[Iterator].Handle == Handle )
				return Iterator;
			}
		return -1;
		};
	for ( unsigned Iterator = 0; Iterator < Command.ShaderBufferBindings.size(); ++Iterator )
		{
		int Index = FindAttributeIndexFromHandle ( Command.ShaderBufferBindings[Iterator].AttributeHandle );
		if ( Index == -1 )
			{
			LOG_ERROR ( "Shader attribute handle does not belong to shader" );
			Result = false;
			continue;
			}
		for ( unsigned Iterator2 = Iterator + 1; Iterator2 < Command.ShaderBufferBindings.size(); ++Iterator2 )
			{
			if ( Command.ShaderBufferBindings[Iterator].AttributeHandle == Command.ShaderBufferBindings[Iterator2].AttributeHandle )
				{
				LOG_ERROR ( "Duplicate binding for attribute '%s'", Info.Attributes[Index].Name.c_str() );
				Result = false;
				break;
				}
			}
		}

	// Find missing textures
	for ( auto &Iterator : Command.TextureBindings )
		{
		if ( !Iterator.BindSettings.Handle )
			{
			LOG_ERROR ( "Missing texture handle for binding '%s'.", Info.Uniforms[Iterator.UniformHandle.GetKey()].Name.c_str() );
			Result = false;
			}
		}

	// Find missing uniform values
	for ( auto &Iterator : Command.UniformValues )
		{
		int Index = FindUniformIndexFromHandle ( Iterator.UniformHandle );
		if ( Index == -1 )
			continue;
		Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
		}
	for ( auto &Iterator : Command.TextureBindings )
		{
		int Index = FindUniformIndexFromHandle ( Iterator.UniformHandle );
		if ( Index == -1 )
			continue;
		Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
		}
	for ( unsigned Iterator = 0; Iterator < Info.Uniforms.size(); ++Iterator )
		{
		LOG_ERROR ( "Missing value for uniform '%s'. Type '%s'", Info.Uniforms[Iterator].Name.c_str(), Stringify ( Info.Uniforms[Iterator].Type ) );
		Result = false;
		}
	// Find missing attribute bindings
	for ( auto &Iterator : Command.ShaderBufferBindings )
		{
		int Index = FindAttributeIndexFromHandle ( Iterator.AttributeHandle );
		if ( Index == -1 )
			continue;
		Info.Attributes.erase ( Info.Attributes.begin() + Index );
		}
	for ( unsigned Iterator = 0; Iterator < Info.Attributes.size(); ++Iterator )
		{
		LOG_ERROR ( "Missing binding for attribute '%s'. Type '%s'", Info.Attributes[Iterator].Name.c_str(), Stringify ( Info.Attributes[Iterator].Type ) );
		Result = false;
		}
	return Result;
	}
}
