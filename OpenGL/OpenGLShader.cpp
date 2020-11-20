#include "OpenGLShader.h"
#include "OpenGLInternals.h"
#include "OpenGLShaderInternals.h"
#include <Logger/Logger.h>

namespace CrossRenderer
{
namespace OpenGL
{
ShaderHandle CreateShader ( const ShaderCode &NewCode )
    {
    ShaderInfo NewShader;
    NewShader.OpenGLID = glCreateProgram();
    if ( CheckError() == false )
        return ShaderHandle::invalid;

    if ( CompileShader ( NewShader.OpenGLID, NewCode ) == false )
        {
        glDeleteProgram ( NewShader.OpenGLID );
        return ShaderHandle::invalid;
        }

    LOG_DEBUG ( "Created shader program %u", NewShader.OpenGLID );

    if ( DetectUniformsAndAttributes ( NewShader.OpenGLID, NewShader.Uniforms, NewShader.Attributes ) == false )
        {
        LOG_ERROR ( "Error detecting uniforms/attributes" );
        glDeleteProgram ( NewShader.OpenGLID );
        return ShaderHandle::invalid;
        }

    ShaderHandle NewHandle = Shaders.GetNewHandle();
    Shaders[NewHandle] = NewShader;
    return NewHandle;
    }

bool DeleteShader ( const ShaderHandle Handle )
    {
    ShaderInfo *info = &Shaders[Handle];

    glDeleteProgram ( info->OpenGLID );
    Shaders.ReleaseHandle ( Handle );
    return true;
    }

bool ChangeShaderCode ( const ShaderHandle Handle, const ShaderCode &NewCode )
    {
    ShaderInfo *info = &Shaders[Handle];

    if ( CompileShader ( info->OpenGLID, NewCode ) == false )
        return false;

    if ( DetectUniformsAndAttributes ( info->OpenGLID, info->Uniforms, info->Attributes ) == false )
        {
        LOG_ERROR ( "Error detecting uniforms/attributes" );
        return false;
        }
    return true;
    }

void GetShaderUniformList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderUniformType>> &UniformList )
    {
    ShaderInfo *info = &Shaders[Handle];

    for ( const auto &iterator : info->Uniforms )
        UniformList.push_back ( std::pair <std::string, ShaderUniformType> ( iterator.Name, iterator.Type ) );
    }

ShaderUniformHandle GetShaderUniformHandle ( const ShaderHandle Handle, const std::string Name )
    {
    ShaderInfo *info = &Shaders[Handle];

    for ( unsigned cont = 0; cont < info->Uniforms.size(); ++cont )
        {
        if ( info->Uniforms[cont].Name == Name )
            return ShaderUniformHandle ( cont );
        }
    LOG_ERROR ( "Invalid uniform '%s' for shader", Name.c_str() );
    return ShaderUniformHandle::invalid;
    }

void GetShaderAttributeList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList )
    {
    ShaderInfo *info = &Shaders[Handle];

    for ( const auto &iterator : info->Attributes )
        AttributeList.push_back ( std::pair <std::string, ShaderAttributeType> ( iterator.Name, iterator.Type ) );
    }

ShaderAttributeHandle GetShaderAttributeHandle ( const ShaderHandle Handle, const std::string Name )
    {
    ShaderInfo *info = &Shaders[Handle];

    for ( unsigned cont = 0; cont < info->Attributes.size(); ++cont )
        {
        if ( info->Attributes[cont].Name == Name )
            return ShaderAttributeHandle ( cont );
        }
    LOG_ERROR ( "Invalid attribute '%s' for shader", Name.c_str() );
    return ShaderAttributeHandle::invalid;
    }

void GetShaderInformation ( const ShaderHandle Handle, ShaderInformation &Information )
    {
    ShaderInfo *info = &Shaders[Handle.key()];
    for ( unsigned cont = 0; cont < info->Uniforms.size(); ++cont )
        {
        ShaderInformation::ShaderUniformInformation Info;
        Info.Name = info->Uniforms[cont].Name;
        Info.Type = info->Uniforms[cont].Type;
        Info.Handle = ShaderUniformHandle ( cont );
        Information.Uniforms.push_back ( Info );
        }
    for ( unsigned cont = 0; cont < info->Attributes.size(); ++cont )
        {
        ShaderInformation::ShaderAttributeInformation Info;
        Info.Name = info->Attributes[cont].Name;
        Info.Type = info->Attributes[cont].Type;
        Info.Handle = ShaderAttributeHandle ( cont );
        Information.Attributes.push_back ( Info );
        }
    Information.Handle = Handle;
    }
}
}
