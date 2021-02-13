#include "OpenGLDSARenderer.h"
#include "OpenGLDSAShaderBuffer.h"
#include "OpenGLInternals.h"
#include "OpenGLStateCache.h"
#include "../Renderer.h"
#include <algorithm>
namespace CrossRenderer
{
namespace OpenGL
{
bool SetShaderUniformValue( const ShaderHandle Handle, const ShaderUniformHandle UniformHandle, const ShaderUniformValue &Value );
bool DSARunCommand ( const RenderCommand &Command )
    {
    // Disable active vertex attributes
    if ( ( ActiveShader ) && ( ActiveShader != Command.Shader ) )
        {
        ShaderInfo *OldShaderInfo = &Shaders[ActiveShader];
        for ( auto &AttributeIterator : OldShaderInfo->Attributes )
            {
            if ( AttributeIterator.Enabled == true )
                {
                AttributeIterator.Enabled = false;
                glDisableVertexAttribArray( AttributeIterator.OpenGLID );
                }
            }
        }
    // Activate new shader
    ShaderInfo *ShaderInformation = &Shaders[Command.Shader];
    glUseProgram( ShaderInformation->OpenGLID );
    ActiveShader = Command.Shader;
    CheckError();

    CurrentState.ApplyState( Command.State );
    CheckError();

    // Sets shader uniform values
    for ( auto &Iterator : Command.UniformValues )
        {
        if ( !Iterator.UniformHandle )
            return false;
        UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.key()];

        if ( UniformInformation->CurrentValue.Equals ( Iterator.UniformValue, UniformInformation->Type ) == true )
            continue;
        SetShaderUniformValue( Command.Shader, Iterator.UniformHandle, Iterator.UniformValue );

        switch ( UniformInformation->Type )
            {
#define CASE_TYPE(TYPENAME,FUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, Iterator.UniformValue.TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, 1, glm::value_ptr(Iterator.UniformValue.TYPENAME##Value) );\
            break;\
            }

                CASE_TYPE ( Float, glProgramUniform1f )
                CASE_TYPE_VEC ( Float2, glProgramUniform2fv )
                CASE_TYPE_VEC ( Float3, glProgramUniform3fv )
                CASE_TYPE_VEC ( Float4, glProgramUniform4fv )

                CASE_TYPE ( Integer,  glProgramUniform1i )
                CASE_TYPE_VEC ( Integer2, glProgramUniform2iv )
                CASE_TYPE_VEC ( Integer3, glProgramUniform3iv )
                CASE_TYPE_VEC ( Integer4, glProgramUniform4iv )

                CASE_TYPE ( UnsignedInteger, glProgramUniform1ui )
                CASE_TYPE_VEC ( UnsignedInteger2, glProgramUniform2uiv )
                CASE_TYPE_VEC ( UnsignedInteger3, glProgramUniform3uiv )
                CASE_TYPE_VEC ( UnsignedInteger4, glProgramUniform4uiv )
#undef CASE_TYPE
#undef CASE_TYPE_VEC

            case ShaderUniformType::Matrix3:
                glProgramUniformMatrix3fv ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Iterator.UniformValue.Matrix3Value ) );
                break;
            case ShaderUniformType::Matrix4:
                glProgramUniformMatrix4fv ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Iterator.UniformValue.Matrix4Value ) );
                break;
            case ShaderUniformType::Bool:
                glProgramUniform1i ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, Iterator.UniformValue.BoolValue );
                break;
            case ShaderUniformType::Block:
                {
                SetShaderUniformValue( Command.Shader, Iterator.UniformHandle, Iterator.UniformValue );
                //ShaderBufferInfo SBInfo = ShaderBuffers[Iterator.UniformValue.BlockValue];
                //glBindBufferBase ( GL_UNIFORM_BUFFER, ShaderInformation->OpenGLID, SBInfo.OpenGLID );
                break;
                }
            default:
                throw std::runtime_error ( "Unknown uniform type" );
            }
        CheckError();
        UniformInformation->CurrentValue = Iterator.UniformValue;
        }
    CheckError();

    // Sets shader attribute bindings
    typedef struct
        {
        typedef struct
            {
            GLuint BufferOpenGLID;
            GLsizei Stride;
            GLint StartOffset;
            } BufferInfo;
        typedef struct
            {
            int BufferIndex;
            GLint AttributeOpenGLID;
            GLint ComponentsPerElement;
            GLint ComponentType;
            GLint StartOffset;
            bool NormalizeData;
            } StreamInfo;
        std::vector <BufferInfo> Buffers;
        std::vector <StreamInfo> Streams;
        } CompiledBindingInformation;

    CompiledBindingInformation BindingInformation;
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        ShaderBufferInfo *BufferInformation = & ( ShaderBuffers[Iterator.DataStream.BufferHandle] );
        CompiledBindingInformation::StreamInfo NewStream;

        auto FindItemByID = [ = ] ( CompiledBindingInformation::BufferInfo Item )
            {
            return Item.BufferOpenGLID == BufferInformation->OpenGLID;
            };
        auto FindResult = std::find_if ( BindingInformation.Buffers.begin(),
                                         BindingInformation.Buffers.end(),
                                         FindItemByID );
        if ( FindResult == BindingInformation.Buffers.end() )
            {
            CompiledBindingInformation::BufferInfo NewBufferInfo;
            NewBufferInfo.BufferOpenGLID = BufferInformation->OpenGLID;
            NewBufferInfo.StartOffset = (GLint) Iterator.DataStream.StartOffset;
            NewBufferInfo.Stride = (GLsizei) Iterator.DataStream.Stride;
            BindingInformation.Buffers.push_back ( NewBufferInfo );

            NewStream.BufferIndex = (int)BindingInformation.Buffers.size() - 1;
            }
        else
            {
            NewStream.BufferIndex = (int)(FindResult - BindingInformation.Buffers.begin());
            }

        AttributeInfo *AttributeInformation = & ( ShaderInformation->Attributes[Iterator.AttributeHandle.key()] );
        NewStream.AttributeOpenGLID = AttributeInformation->OpenGLID;
        NewStream.ComponentsPerElement = (GLint)Iterator.DataStream.ComponentsPerElement;
        NewStream.ComponentType = Translate ( Iterator.DataStream.ComponentType );
        NewStream.NormalizeData = Iterator.DataStream.NormalizeData;
        NewStream.StartOffset = (GLint)Iterator.DataStream.StartOffset;
        BindingInformation.Streams.push_back ( NewStream );
        }

    GLint LastIndex = 0;
    for ( auto &Iterator : BindingInformation.Buffers )
        {
        glBindVertexBuffer ( LastIndex++, Iterator.BufferOpenGLID, Iterator.StartOffset, Iterator.Stride );
        }

    for ( auto &Iterator : BindingInformation.Streams )
        {
        glEnableVertexAttribArray ( Iterator.AttributeOpenGLID );
        glVertexAttribFormat ( Iterator.AttributeOpenGLID,
                               Iterator.ComponentsPerElement,
                               Iterator.ComponentType,
                               Iterator.NormalizeData,
                               Iterator.StartOffset );
        glVertexAttribBinding ( Iterator.AttributeOpenGLID, Iterator.BufferIndex );
        }
    CheckError();

    // Sets texture bindings
    unsigned TextureLevel = 0;
    for ( auto &Iterator : Command.TextureBindings )
        {
        UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.key()];

        GLint DesiredSWrap = Translate ( Iterator.BindSettings.WrapSettings.Horizontal );
        GLint DesiredTWrap = Translate ( Iterator.BindSettings.WrapSettings.Vertical );
        GLint DesiredMinFilter = Translate ( Iterator.BindSettings.FilterSettings.MinFilter );
        GLint DesiredMagFilter = Translate ( Iterator.BindSettings.FilterSettings.MagFilter );

        TextureInfo *TextureInformation = &Textures[Iterator.BindSettings.Handle];
        glBindTextureUnit ( TextureLevel, TextureInformation->OpenGLID );

        if ( TextureInformation->GLSWrap != DesiredSWrap )
            glTextureParameteri ( TextureInformation->OpenGLID, GL_TEXTURE_WRAP_S, DesiredSWrap );
        if ( TextureInformation->GLTWrap != DesiredTWrap )
            glTextureParameteri ( TextureInformation->OpenGLID, GL_TEXTURE_WRAP_T, DesiredTWrap );
        if ( TextureInformation->GLMinFilter != DesiredMinFilter )
            glTextureParameteri ( TextureInformation->OpenGLID, GL_TEXTURE_MIN_FILTER, DesiredMinFilter );
        if ( TextureInformation->GLMagFilter != DesiredMagFilter )
            glTextureParameteri ( TextureInformation->OpenGLID, GL_TEXTURE_MAG_FILTER, DesiredMagFilter );
        glProgramUniform1i ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, ( GLint ) TextureLevel );

        if ( CheckError() == false )
            {
            LOG_ERROR ( "Error activating texture" );
            return false;
            }
        TextureInformation->GLSWrap = DesiredSWrap;
        TextureInformation->GLTWrap = DesiredTWrap;
        TextureInformation->GLMinFilter = DesiredMinFilter;
        TextureInformation->GLMagFilter = DesiredMagFilter;

        ++TextureLevel;
        }
    CheckError();

    // Issues the render call
    if ( Command.IndexBuffer )
        {
        static size_t Sizes[] = { sizeof ( float ), sizeof ( uint8_t ), sizeof ( uint16_t ), sizeof ( uint32_t ) };

        ShaderBufferInfo *BufferToUse = &ShaderBuffers[Command.IndexBuffer];
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, BufferToUse->OpenGLID );
        glDrawElements ( Translate ( Command.Primitive ),
                         ( GLint ) Command.VertexCount,
                         Translate ( Command.IndexBufferStream.ComponentType ),
                         reinterpret_cast <void *> ( Command.StartVertex * Sizes[ ( int ) Command.IndexBufferStream.ComponentType] ) );
        }
    else
        {
        glDrawArrays ( Translate ( Command.Primitive ), ( GLint ) Command.StartVertex, static_cast <GLsizei> ( Command.VertexCount ) );
        }
    return CheckError();
    }
}
}
