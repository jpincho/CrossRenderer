#include <stdexcept>
#include "OpenGLShaderInternals.h"
#include "OpenGLInternals.h"
#include <Logger/Logger.h>

namespace CrossRenderer
{
namespace OpenGL
{

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
        default:
            return ShaderUniformType::Unknown;
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

bool BuildShaderObject ( unsigned &Out_ShaderObjectGLID, const unsigned ShaderType, const std::string &ShaderString )
    {
    // Groups the sources together into an array of char pointers
    std::vector <const char*> Sources;
    Sources.push_back ( ShaderString.c_str() );

    std::string InfoLog;
    int CompileStatus = 0;

    LOG_DEBUG ( "Compiling shader object" );
    Out_ShaderObjectGLID = glCreateShader ( ShaderType );

    // Loads the shader source code
    glShaderSource ( Out_ShaderObjectGLID, ( GLsizei ) Sources.size(), Sources.data(), nullptr );
    if ( CheckError() == false )
        goto OnError;

    glCompileShader ( Out_ShaderObjectGLID );
    if ( CheckError() == false )
        goto OnError;

    glGetShaderiv ( Out_ShaderObjectGLID, GL_COMPILE_STATUS, &CompileStatus );
    if ( CheckError() == false )
        goto OnError;

    InfoLog = GetInfoLog ( false, Out_ShaderObjectGLID );
    if ( CheckError() == false )
        goto OnError;

    if ( CompileStatus != GL_TRUE )
        {
        LOG_ERROR ( "Infolog - %s", InfoLog.c_str() );
        goto OnError;
        }
    if ( CheckError() == false )
        goto OnError;

    LOG_DEBUG ( "Shader object successfully compiled" );
    return true;
OnError:
    LOG_ERROR ( "OpenGL error during shader object compilation" );
    glDeleteShader ( Out_ShaderObjectGLID );
    return false;
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

bool DSASetShaderUniformValue ( const ShaderHandle Handle, const ShaderUniformHandle UniformHandle, const ShaderUniformValue &Value )
    {
    ShaderInfo *info = &Shaders[Handle];

    if ( !UniformHandle )
        return false;
    UniformInfo *Uniform = &info->Uniforms[UniformHandle.key()];

    if ( Value.Type != Uniform->Type )
        {
        LOG_ERROR ( "Mismatching type for uniform '%s'", Uniform->Name.c_str() );
        return false;
        }
    switch ( Uniform->Type )
        {
#define CASE_TYPE(TYPENAME,FUNCTION,DSAFUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            DSAFUNCTION ( info->OpenGLID, Uniform->OpenGLID, Value.TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION,DSAFUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            DSAFUNCTION ( info->OpenGLID, Uniform->OpenGLID, 1, glm::value_ptr(Value.TYPENAME##Value) );\
            break;\
            }

            CASE_TYPE ( Float, glUniform1f, glProgramUniform1f )
            CASE_TYPE_VEC ( Float2, glUniform2fv, glProgramUniform2fv )
            CASE_TYPE_VEC ( Float3, glUniform3fv, glProgramUniform3fv )
            CASE_TYPE_VEC ( Float4, glUniform4fv, glProgramUniform4fv )

            CASE_TYPE ( Integer, glUniform1i, glProgramUniform1i )
            CASE_TYPE_VEC ( Integer2, glUniform2iv, glProgramUniform2iv )
            CASE_TYPE_VEC ( Integer3, glUniform3iv, glProgramUniform3iv )
            CASE_TYPE_VEC ( Integer4, glUniform4iv, glProgramUniform4iv )

            CASE_TYPE ( UnsignedInteger, glUniform1ui, glProgramUniform1ui )
            CASE_TYPE_VEC ( UnsignedInteger2, glUniform2uiv, glProgramUniform2uiv )
            CASE_TYPE_VEC ( UnsignedInteger3, glUniform3uiv, glProgramUniform3uiv )
            CASE_TYPE_VEC ( UnsignedInteger4, glUniform4uiv, glProgramUniform4uiv )
#undef CASE_TYPE
#undef CASE_TYPE_VEC

        case ShaderUniformType::Matrix3:
            glProgramUniformMatrix3fv ( info->OpenGLID, Uniform->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Value.Matrix3Value ) );
            break;
        case ShaderUniformType::Matrix4:
            glProgramUniformMatrix4fv ( info->OpenGLID, Uniform->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Value.Matrix4Value ) );
            break;
        case ShaderUniformType::Bool:
            glProgramUniform1i ( info->OpenGLID, Uniform->OpenGLID, Value.BoolValue );
            break;
        case ShaderUniformType::Block:
            {
            ShaderBufferInfo SBInfo = ShaderBuffers[Value.BlockValue];
            glBindBufferBase ( GL_UNIFORM_BUFFER, Uniform->OpenGLID, SBInfo.OpenGLID );
            break;
            }
        default:
            throw std::runtime_error ( "Unknown uniform type" );
        }
    return CheckError();
    }

bool SetShaderUniformValue ( const ShaderHandle Handle, const ShaderUniformHandle UniformHandle, const ShaderUniformValue &Value )
    {
    ShaderInfo *info = &Shaders[Handle];

    if ( !UniformHandle )
        return false;
    UniformInfo *Uniform = &info->Uniforms[UniformHandle.key()];

    if ( Value.Type != Uniform->Type )
        {
        LOG_ERROR ( "Mismatching type for uniform '%s'", Uniform->Name.c_str() );
        return false;
        }
    switch ( Uniform->Type )
        {
#define CASE_TYPE(TYPENAME,FUNCTION,DSAFUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( Uniform->OpenGLID, Value.TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION,DSAFUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( Uniform->OpenGLID, 1, glm::value_ptr(Value.TYPENAME##Value) );\
            break;\
            }

            CASE_TYPE ( Float, glUniform1f, glProgramUniform1f )
            CASE_TYPE_VEC ( Float2, glUniform2fv, glProgramUniform2fv )
            CASE_TYPE_VEC ( Float3, glUniform3fv, glProgramUniform3fv )
            CASE_TYPE_VEC ( Float4, glUniform4fv, glProgramUniform4fv )

            CASE_TYPE ( Integer, glUniform1i, glProgramUniform1i )
            CASE_TYPE_VEC ( Integer2, glUniform2iv, glProgramUniform2iv )
            CASE_TYPE_VEC ( Integer3, glUniform3iv, glProgramUniform3iv )
            CASE_TYPE_VEC ( Integer4, glUniform4iv, glProgramUniform4iv )

            CASE_TYPE ( UnsignedInteger, glUniform1ui, glProgramUniform1ui )
            CASE_TYPE_VEC ( UnsignedInteger2, glUniform2uiv, glProgramUniform2uiv )
            CASE_TYPE_VEC ( UnsignedInteger3, glUniform3uiv, glProgramUniform3uiv )
            CASE_TYPE_VEC ( UnsignedInteger4, glUniform4uiv, glProgramUniform4uiv )
#undef CASE_TYPE
#undef CASE_TYPE_VEC

        case ShaderUniformType::Matrix3:
            glUniformMatrix3fv ( Uniform->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Value.Matrix3Value ) );
            break;
        case ShaderUniformType::Matrix4:
            glUniformMatrix4fv ( Uniform->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Value.Matrix4Value ) );
            break;
        case ShaderUniformType::Bool:
            glUniform1i ( Uniform->OpenGLID, Value.BoolValue );
            break;
        case ShaderUniformType::Block:
            {
            ShaderBufferInfo SBInfo = ShaderBuffers[Value.BlockValue];
            glBindBufferBase ( GL_UNIFORM_BUFFER, Uniform->OpenGLID, SBInfo.OpenGLID );
            break;
            }
        default:
            throw std::runtime_error ( "Unknown uniform type" );
        }
    return CheckError();
    }
}
}
