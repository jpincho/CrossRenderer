#include "Direct3D11Shader.hpp"
#include "Direct3D11Internals.hpp"
#include "../Internal/Logger.hpp"
#include "../Internal/ConsoleTextColor.hpp"
#include "../Internal/StringUtils.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
ShaderObjectHandle CreateShaderObject ( const ShaderObjectType )
	{
	return ShaderObjectHandle::Invalid;
	}

void DeleteShaderObject ( const ShaderObjectHandle )
	{
	}

bool BuildShaderObject ( const ShaderObjectHandle, const std::string & )
	{
	return false;
	}

ShaderHandle CreateShader ( void )
	{
	ShaderInfo NewShader;
	ShaderHandle NewHandle ( Shaders.GetFreeIndex () );
	Shaders[NewHandle.GetKey ()] = NewShader;
	return NewHandle;
	}

ShaderHandle CreateShader ( const ShaderCode &Code )
	{
	ShaderHandle NewHandle = CreateShader ();

	ID3D11VertexShader *VertexShader = nullptr;
	ID3D11PixelShader *FragmentShader = nullptr;
	ID3D11GeometryShader *GeometryShader = nullptr;

	ID3DBlob *VertexShaderBlob = nullptr, *FragmentShaderBlob = nullptr, *GeometryShaderBlob = nullptr,  *ErrorMessageBlob = nullptr;
	HRESULT CompilationResult = D3DCompile (
			Code.VertexShader.c_str (),
			Code.VertexShader.length (),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			"vs_5_0",
			0,
			0,
			&VertexShaderBlob,
			&ErrorMessageBlob );
	if ( CompilationResult != S_OK )
		goto OnError;
	CompilationResult = D3DCompile (
			Code.GeometryShader.c_str (),
			Code.GeometryShader.length (),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			"gs_5_0",
			0,
			0,
			&GeometryShaderBlob,
			&ErrorMessageBlob );
	if ( CompilationResult != S_OK )
		goto OnError;
	CompilationResult = D3DCompile (
			Code.FragmentShader.c_str (),
			Code.FragmentShader.length (),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			"fs_5_0",
			0,
			0,
			&FragmentShaderBlob,
			&ErrorMessageBlob );
	if ( CompilationResult != S_OK )
		goto OnError;
	return NewHandle;
OnError:
	if ( VertexShader )
		VertexShader->Release ();
	if ( FragmentShader )
		FragmentShader->Release ();
	if ( GeometryShader )
		GeometryShader->Release ();
	if ( VertexShaderBlob )
		VertexShaderBlob->Release ();
	if ( FragmentShaderBlob )
		FragmentShaderBlob->Release ();
	if ( GeometryShaderBlob )
		GeometryShaderBlob->Release ();
	if ( NewHandle )
		DeleteShader ( NewHandle );
	return ShaderHandle::Invalid;
	}

bool DeleteShader ( const ShaderHandle )
	{
	return false;
	}

bool LinkShader ( const ShaderHandle, const std::vector <ShaderObjectHandle> & )
	{
	return false;
	}

void GetShaderUniformList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderUniformType>> &UniformList )
	{
	ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

	for ( const auto &iterator : ShaderInformation->Uniforms )
		UniformList.push_back ( std::pair <std::string, ShaderUniformType> ( iterator.Name, iterator.Type ) );
	}

ShaderUniformHandle GetShaderUniformHandle ( const ShaderHandle Handle, const std::string Name )
	{
	ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

	for ( unsigned cont = 0; cont < ShaderInformation->Uniforms.size (); ++cont )
		{
		if ( ShaderInformation->Uniforms[cont].Name == Name )
			return ShaderUniformHandle ( cont );
		}
	//LOG_ERROR ( "Invalid uniform '%s' for shader", Name.c_str() );
	return ShaderUniformHandle::Invalid;
	}

void GetShaderAttributeList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList )
	{
	ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

	for ( const auto &iterator : ShaderInformation->Attributes )
		AttributeList.push_back ( std::pair <std::string, ShaderAttributeType> ( iterator.Name, iterator.Type ) );
	}

ShaderAttributeHandle GetShaderAttributeHandle ( const ShaderHandle Handle, const std::string Name )
	{
	ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];

	for ( unsigned cont = 0; cont < ShaderInformation->Attributes.size (); ++cont )
		{
		if ( ShaderInformation->Attributes[cont].Name == Name )
			return ShaderAttributeHandle ( cont );
		}
	//LOG_ERROR ( "Invalid attribute '%s' for shader", Name.c_str() );
	return ShaderAttributeHandle::Invalid;
	}

void GetShaderInformation ( const ShaderHandle Handle, ShaderInformation &Information )
	{
	ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey ()];
	for ( unsigned cont = 0; cont < ShaderInformation->Uniforms.size (); ++cont )
		{
		ShaderInformation::ShaderUniformInformation Info;
		Info.Name = ShaderInformation->Uniforms[cont].Name;
		Info.Type = ShaderInformation->Uniforms[cont].Type;
		Info.Handle = ShaderUniformHandle ( cont );
		Information.Uniforms.push_back ( Info );
		}
	for ( unsigned cont = 0; cont < ShaderInformation->Attributes.size (); ++cont )
		{
		ShaderInformation::ShaderAttributeInformation Info;
		Info.Name = ShaderInformation->Attributes[cont].Name;
		Info.Type = ShaderInformation->Attributes[cont].Type;
		Info.Handle = ShaderAttributeHandle ( cont );
		Information.Attributes.push_back ( Info );
		}
	Information.AttachedShaderObjects = ShaderInformation->AttachedShaderObjects;
	Information.Handle = Handle;
	}
}
}
