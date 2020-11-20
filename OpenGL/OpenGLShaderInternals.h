#pragma once
#include "OpenGLInternals.h"
#include <vector>

namespace CrossRenderer
{
namespace OpenGL
{
ShaderUniformType TranslateOpenGLUniformType ( GLenum Type );
std::string GetInfoLog ( bool IsProgram, GLuint GLID );
bool BuildShaderObject ( unsigned &Out_ShaderObjectGLID, const unsigned ShaderType, const std::string &ShaderString );
bool CompileShader ( const GLuint OpenGLID, const ShaderCode &NewCode );
bool DetectUniformsAndAttributes ( GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes );
bool SetShaderUniformValue ( const ShaderHandle Handle, const ShaderUniformHandle UniformHandle, const ShaderUniformValue &Value );
bool DSASetShaderUniformValue ( const ShaderHandle Handle, const ShaderUniformHandle UniformHandle, const ShaderUniformValue &Value );
}
}
