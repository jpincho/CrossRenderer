#pragma once
#include "../ShaderDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderHandle CreateShader ( const ShaderCode &NewCode );
bool DeleteShader ( const ShaderHandle Handle );
bool ChangeShaderCode ( const ShaderHandle Handle, const ShaderCode &NewCode );
void GetShaderUniformList ( const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList );
ShaderUniformHandle GetShaderUniformHandle ( const ShaderHandle Handle, const std::string Name );
void GetShaderAttributeList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList );
ShaderAttributeHandle GetShaderAttributeHandle ( const ShaderHandle Handle, const std::string Name );
void GetShaderInformation ( const ShaderHandle Handle, ShaderInformation &Information );
}
}
