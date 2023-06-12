#pragma once
#include "../ShaderDefinitions.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
ShaderObjectHandle CreateShaderObject ( const ShaderObjectType Type );
void DeleteShaderObject ( const ShaderObjectHandle Handle );
bool BuildShaderObject ( const ShaderObjectHandle Handle, const std::string &Code );
ShaderHandle CreateShader ( const ShaderCode &NewCode );
bool DeleteShader ( const ShaderHandle Handle );
bool LinkShader ( const ShaderHandle Handle, const std::vector <ShaderObjectHandle> &ObjectHandles );
void GetShaderUniformList ( const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList );
ShaderUniformHandle GetShaderUniformHandle ( const ShaderHandle Handle, const std::string Name );
void GetShaderAttributeList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList );
ShaderAttributeHandle GetShaderAttributeHandle ( const ShaderHandle Handle, const std::string Name );
void GetShaderInformation ( const ShaderHandle Handle, ShaderInformation &Information );
}
}
