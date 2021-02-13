#include "OpenGLShader.h"
#include "OpenGLInternals.h"
#include "../Logger.h"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderUniformType TranslateOpenGLUniformType ( GLenum Type );
std::string GetInfoLog ( bool IsProgram, GLuint GLID );
bool BuildShaderObject ( unsigned &Out_ShaderObjectGLID, const unsigned ShaderType, const std::string &ShaderString );
bool CompileShader ( const GLuint OpenGLID, const ShaderCode &NewCode );
bool DetectUniformsAndAttributes ( GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes );

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
    ShaderInfo *ShaderInformation = &Shaders[Handle];

    glDeleteProgram ( ShaderInformation->OpenGLID );
    Shaders.ReleaseHandle ( Handle );
    return true;
    }

bool ChangeShaderCode ( const ShaderHandle Handle, const ShaderCode &NewCode )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle];

    if ( CompileShader ( ShaderInformation->OpenGLID, NewCode ) == false )
        return false;

    if ( DetectUniformsAndAttributes ( ShaderInformation->OpenGLID, ShaderInformation->Uniforms, ShaderInformation->Attributes ) == false )
        {
        LOG_ERROR ( "Error detecting uniforms/attributes" );
        return false;
        }
    return true;
    }

void GetShaderUniformList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderUniformType>> &UniformList )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle];

    for ( const auto &iterator : ShaderInformation->Uniforms )
        UniformList.push_back ( std::pair <std::string, ShaderUniformType> ( iterator.Name, iterator.Type ) );
    }

ShaderUniformHandle GetShaderUniformHandle ( const ShaderHandle Handle, const std::string Name )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle];

    for ( unsigned cont = 0; cont < ShaderInformation->Uniforms.size(); ++cont )
        {
        if ( ShaderInformation->Uniforms[cont].Name == Name )
            return ShaderUniformHandle ( cont );
        }
    LOG_ERROR ( "Invalid uniform '%s' for shader", Name.c_str() );
    return ShaderUniformHandle::invalid;
    }

void GetShaderAttributeList ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle];

    for ( const auto &iterator : ShaderInformation->Attributes )
        AttributeList.push_back ( std::pair <std::string, ShaderAttributeType> ( iterator.Name, iterator.Type ) );
    }

ShaderAttributeHandle GetShaderAttributeHandle ( const ShaderHandle Handle, const std::string Name )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle];

    for ( unsigned cont = 0; cont < ShaderInformation->Attributes.size(); ++cont )
        {
        if ( ShaderInformation->Attributes[cont].Name == Name )
            return ShaderAttributeHandle ( cont );
        }
    LOG_ERROR ( "Invalid attribute '%s' for shader", Name.c_str() );
    return ShaderAttributeHandle::invalid;
    }

void GetShaderInformation ( const ShaderHandle Handle, ShaderInformation &Information )
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.key()];
    for ( unsigned cont = 0; cont < ShaderInformation->Uniforms.size(); ++cont )
        {
        ShaderInformation::ShaderUniformInformation Info;
        Info.Name = ShaderInformation->Uniforms[cont].Name;
        Info.Type = ShaderInformation->Uniforms[cont].Type;
        Info.Handle = ShaderUniformHandle ( cont );
        Information.Uniforms.push_back ( Info );
        }
    for ( unsigned cont = 0; cont < ShaderInformation->Attributes.size(); ++cont )
        {
        ShaderInformation::ShaderAttributeInformation Info;
        Info.Name = ShaderInformation->Attributes[cont].Name;
        Info.Type = ShaderInformation->Attributes[cont].Type;
        Info.Handle = ShaderAttributeHandle ( cont );
        Information.Attributes.push_back ( Info );
        }
    Information.Handle = Handle;
    }

ShaderUniformType TranslateOpenGLUniformType ( GLenum Type )
    {
    switch ( Type )
        {
        case GL_FLOAT:
            return ShaderUniformType::Float;
        case GL_FLOAT_VEC2:
            return ShaderUniformType::Float2;
        case GL_FLOAT_VEC3:
            return ShaderUniformType::Float3;
        case GL_FLOAT_VEC4:
            return ShaderUniformType::Float4;

        case GL_INT:
            return ShaderUniformType::Integer;
        case GL_INT_VEC2:
            return ShaderUniformType::Integer2;
        case GL_INT_VEC3:
            return ShaderUniformType::Integer3;
        case GL_INT_VEC4:
            return ShaderUniformType::Integer4;

        case GL_UNSIGNED_INT:
            return ShaderUniformType::UnsignedInteger;
#if defined GL_UNSIGNED_INT_VEC2
        case GL_UNSIGNED_INT_VEC2:
            return ShaderUniformType::UnsignedInteger2;
        case GL_UNSIGNED_INT_VEC3:
            return ShaderUniformType::UnsignedInteger3;
        case GL_UNSIGNED_INT_VEC4:
            return ShaderUniformType::UnsignedInteger4;
#endif
        case GL_BOOL:
            return ShaderUniformType::Bool;
        case GL_BOOL_VEC2:
            return ShaderUniformType::Bool2;
        case GL_BOOL_VEC3:
            return ShaderUniformType::Bool3;
        case GL_BOOL_VEC4:
            return ShaderUniformType::Bool4;

        case GL_FLOAT_MAT2:
            return ShaderUniformType::Matrix2;
        case GL_FLOAT_MAT3:
            return ShaderUniformType::Matrix3;
        case GL_FLOAT_MAT4:
            return ShaderUniformType::Matrix4;
        case GL_SAMPLER_2D:
            return ShaderUniformType::Sampler2D;
        case GL_SAMPLER_3D:
            return ShaderUniformType::Sampler3D;
        case GL_SAMPLER_CUBE:
            return ShaderUniformType::SamplerCube;
        default:
            throw std::runtime_error ( "Unhandled shader uniform type" );
        }
    }

std::string GetInfoLog ( bool IsProgram, GLuint GLID )
    {
    std::string Result;
    int InfologSize = 0;
    CheckError();
    if ( IsProgram )
        glGetProgramiv ( GLID, GL_INFO_LOG_LENGTH, &InfologSize );
    else
        glGetShaderiv ( GLID, GL_INFO_LOG_LENGTH, &InfologSize );

    CheckError();
    InfologSize = std::min ( InfologSize, 1024 );
    if ( InfologSize > 0 )
        {
        Result.resize ( InfologSize );
        if ( IsProgram )
            glGetProgramInfoLog ( GLID, InfologSize, &InfologSize, const_cast <char *> ( Result.data() ) );
        else
            glGetShaderInfoLog ( GLID, InfologSize, &InfologSize, const_cast <char *> ( Result.data() ) );
        }
    CheckError();
    return Result;
    }

bool BuildShaderObject ( unsigned &ShaderObjectGLID, const unsigned ShaderType, const std::string &ShaderString )
    {
    int CompileStatus = 0;

    LOG_DEBUG ( "Compiling shader object" );
    ShaderObjectGLID = glCreateShader ( ShaderType );

    // Loads the shader source code
    const char *Sources[1];
    Sources[0] = ShaderString.c_str();

    glShaderSource ( ShaderObjectGLID, ( GLsizei ) 1, Sources, nullptr );
    glCompileShader ( ShaderObjectGLID );
    glGetShaderiv ( ShaderObjectGLID, GL_COMPILE_STATUS, &CompileStatus );

    char Infolog[1024];
    glGetShaderInfoLog ( ShaderObjectGLID, 1024, nullptr, Infolog );
    if ( CheckError() == false )
        {
        LOG_ERROR ( "OpenGL error during shader object compilation" );
        glDeleteShader ( ShaderObjectGLID );
        return false;
        }

    if ( CompileStatus != GL_TRUE )
        {
        LOG_ERROR ( "Compilation failed - %s", Infolog );
        glDeleteShader ( ShaderObjectGLID );
        return false;
        }

    LOG_DEBUG ( "Shader object successfully compiled" );
    return true;
    }

bool CompileShader ( const GLuint OpenGLID, const ShaderCode &NewCode )
    {
    GLuint VertexShaderGLID = 0, GeometryShaderGLID = 0, FragmentShaderGLID = 0;
    std::vector <GLuint> GLIDs;
    if ( BuildShaderObject ( VertexShaderGLID, GL_VERTEX_SHADER, NewCode.VertexShader ) == false )
        return ShaderHandle::invalid;
    GLIDs.push_back ( VertexShaderGLID );
    if ( NewCode.GeometryShader.size() != 0 )
        {
        if ( BuildShaderObject ( GeometryShaderGLID, GL_GEOMETRY_SHADER, NewCode.GeometryShader ) == false )
            goto error;
        GLIDs.push_back ( GeometryShaderGLID );
        }
    if ( BuildShaderObject ( FragmentShaderGLID, GL_FRAGMENT_SHADER, NewCode.FragmentShader ) == false )
        goto error;
    GLIDs.push_back ( FragmentShaderGLID );

    for ( auto &id : GLIDs )
        glAttachShader ( OpenGLID, id );

    // Link it, and check it
    glLinkProgram ( OpenGLID );

    GLint LinkStatus;
    glGetProgramiv ( OpenGLID, GL_LINK_STATUS, &LinkStatus );
    if ( LinkStatus != GL_TRUE )
        {
        std::string InfoLog = GetInfoLog ( true, OpenGLID );
        LOG_ERROR ( "Shader program %u link error. %s", OpenGLID, InfoLog.c_str() );
        goto error;
        }
    LOG_DEBUG ( "Linked shader program %u", OpenGLID );

    for ( auto &id : GLIDs )
        glDeleteShader ( id );

    return true;

error:
    for ( auto &id : GLIDs )
        glDeleteShader ( id );
    return false;
    }

bool DetectUniformsAndAttributes ( GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes )
    {
    GLint UniformCount, MaxUniformNameLength, UniformBlockCount;
    GLint AttributeCount, MaxAttributeNameLength;
    bool Result = false;

    glGetProgramiv ( OpenGLID, GL_ACTIVE_UNIFORMS, &UniformCount );
    glGetProgramiv ( OpenGLID, GL_ACTIVE_ATTRIBUTES, &AttributeCount );
    glGetProgramiv ( OpenGLID, GL_ACTIVE_UNIFORM_BLOCKS, &UniformBlockCount );

    if ( !CheckError() )
        return false;
    if ( ( UniformCount == 0 ) && ( AttributeCount == 0 ) && ( UniformBlockCount == 0 ) )
        return true;

    glGetProgramiv ( OpenGLID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MaxUniformNameLength );
    glGetProgramiv ( OpenGLID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &MaxAttributeNameLength );
    if ( !CheckError() )
        return false;

    size_t NameLength = std::max ( MaxUniformNameLength, MaxAttributeNameLength );
    char *Name = new char [NameLength];

    Uniforms.clear();
    for ( int cont = 0; cont < UniformBlockCount; ++cont )
        {
        GLint Location;

        glGetActiveUniformBlockName ( OpenGLID, cont, ( GLsizei ) NameLength, nullptr, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( strncmp ( "gl_", Name, 3 ) == 0 )
            continue;

        Location = glGetUniformBlockIndex ( OpenGLID, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( Location == -1 )
            {
            LOG_ERROR ( "Unable to get location for uniform block '%s'", Name );
            continue;
            }

        UniformInfo NewUniform;
        NewUniform.Name.assign ( Name );
        NewUniform.Type = ShaderUniformType::Block;
        NewUniform.OpenGLID = Location;
        Uniforms.push_back ( NewUniform );
        }

    for ( int cont = 0; cont < UniformCount; ++cont )
        {
        GLenum GLType;
        GLint Location;
        GLint UniformSize;
        ShaderUniformType Type;

        glGetActiveUniform ( OpenGLID, cont, ( GLsizei ) NameLength, nullptr, &UniformSize, &GLType, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( strncmp ( "gl_", Name, 3 ) == 0 )
            continue;

        Location = glGetUniformLocation ( OpenGLID, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( Location == -1 )
            {
            // TODO Check if this is part of an uniform block
            LOG_ERROR ( "Unable to get location for uniform '%s'", Name );
            continue;
            }

        Type = TranslateOpenGLUniformType ( GLType );
        UniformInfo NewUniform;
        NewUniform.Name.assign ( Name );
        NewUniform.Type = Type;
        NewUniform.OpenGLID = Location;
        Uniforms.push_back ( NewUniform );
        }

    Attributes.clear();
    for ( int cont = 0; cont < AttributeCount; ++cont )
        {
        GLenum GLType;
        GLint Location;
        GLint AttributeSize;
        ShaderAttributeType Type;

        glGetActiveAttrib ( OpenGLID, cont, ( GLsizei ) NameLength, nullptr, &AttributeSize, &GLType, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( strncmp ( "gl_", Name, 3 ) == 0 )
            continue;

        Location = glGetAttribLocation ( OpenGLID, Name );
        if ( !CheckError() )
            goto cleanup;
        if ( Location == -1 )
            {
            LOG_ERROR ( "Unable to get location for attribute '%s'", Name );
            goto cleanup;
            }

        Type = TranslateOpenGLUniformType ( GLType );
        AttributeInfo NewAttribute;
        NewAttribute.Name.assign ( Name );
        NewAttribute.Type = Type;
        NewAttribute.OpenGLID = Location;
        NewAttribute.Enabled = false;
        Attributes.push_back ( NewAttribute );
        }
    Result = true;

cleanup:
    delete[] Name;
    return Result;
    }


}
}
