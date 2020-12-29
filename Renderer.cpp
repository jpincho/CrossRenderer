#include "Renderer.h"

namespace CrossRenderer
{
bool ( *InitializeRenderer ) ( const RendererConfiguration &NewConfiguration ) = 0;
bool ( *ShutdownRenderer ) ( void ) = 0;

bool ( *StartRenderToFramebuffer ) ( const FramebufferHandle &Handle ) = 0;
bool ( *DisplayFramebuffer ) ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle ) = 0;

bool ( *StartFrame ) ( const RenderWindowHandle &Handle ) = 0;
bool ( *EndFrame ) ( const RenderWindowHandle &Handle ) = 0;

bool ( *RunCommand ) ( const RenderCommand &Command ) = 0;

ShaderBufferHandle ( *CreateShaderBuffer ) ( const ShaderBufferDescriptor CreationParameters ) = 0;
bool ( *DeleteShaderBuffer ) ( const ShaderBufferHandle Handle ) = 0;
bool ( *ChangeShaderBufferContents ) ( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters ) = 0;

TextureHandle ( *CreateTexture ) ( const TextureDescriptor CreationParameters ) = 0;
bool ( *DeleteTexture ) ( const TextureHandle Handle ) = 0;
bool ( *Load2DTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data ) = 0;
bool ( *Update2DTextureRegion ) ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data ) = 0;
bool ( *LoadCubeMapTextureData ) ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] ) = 0;
glm::uvec2 ( *GetTextureDimensions ) ( const TextureHandle Handle ) = 0;

ShaderHandle ( *CreateShader ) ( const ShaderCode &NewCode ) = 0;
bool ( *DeleteShader ) ( const ShaderHandle Handle ) = 0;
bool ( *ChangeShaderCode ) ( const ShaderHandle Handle, const ShaderCode &NewCode ) = 0;
void ( *GetShaderUniformList ) ( const ShaderHandle Handle, std::vector<std::pair<std::string, ShaderUniformType> > &UniformList ) = 0;
ShaderUniformHandle ( *GetShaderUniformHandle ) ( const ShaderHandle Handle, const std::string Name ) = 0;
void ( *GetShaderAttributeList ) ( const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList ) = 0;
ShaderAttributeHandle ( *GetShaderAttributeHandle ) ( const ShaderHandle Handle, const std::string Name ) = 0;
void ( *GetShaderInformation ) ( const ShaderHandle Handle, ShaderInformation &Information ) = 0;

FramebufferHandle ( *CreateFramebuffer ) ( const FramebufferDescriptor CreationParameters ) = 0;
bool ( *DeleteFramebuffer ) ( const FramebufferHandle Handle ) = 0;
glm::uvec2 ( *GetFramebufferSize ) ( const FramebufferHandle Handle ) = 0;
TextureHandle ( *GetColorBufferFromFramebuffer ) ( const FramebufferHandle Handle, const size_t Index ) = 0;
TextureHandle ( *GetDepthBufferFromFramebuffer ) ( const FramebufferHandle Handle ) = 0;

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
    for ( auto& Iterator : Command.UniformValues )
        {
        if ( Iterator.UniformHandle == ShaderUniformHandle::invalid )
            {
            LOG_ERROR ( "Invalid uniform handle" );
            Result = false;
            break;
            }
        }

    // Find invalid attribute handles
    for ( auto& Iterator : Command.ShaderBufferBindings )
        {
        if ( Iterator.AttributeHandle == ShaderAttributeHandle::invalid )
            {
            LOG_ERROR ( "Invalid attribute handle" );
            Result = false;
            break;
            }
        }

    // Find invalid texture uniform handles
    for ( auto& Iterator : Command.TextureBindings )
        {
        if ( Iterator.UniformHandle == ShaderUniformHandle::invalid )
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
        if ( Command.UniformValues[Iterator].UniformValue.Type != Info.Uniforms[Index].Type )
            {
            LOG_ERROR ( "Wrong value type for uniform '%s'", Info.Uniforms[Index].Name.c_str() );
            Result = false;
            continue;
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

    // Find missing uniform values
    for ( auto& Iterator : Command.UniformValues )
        {
        int Index = FindUniformIndexFromHandle ( Iterator.UniformHandle );
        if ( Index == -1 )
            continue;
        Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
        }
    for ( auto& Iterator : Command.TextureBindings )
        {
        int Index = FindUniformIndexFromHandle ( Iterator.UniformHandle );
        if ( Index == -1 )
            continue;
        Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
        }
    for ( unsigned Iterator = 0; Iterator < Info.Uniforms.size(); ++Iterator )
        {
        LOG_ERROR ( "Missing value for uniform '%s'. Type '%s'", Info.Uniforms[Iterator].Name.c_str(), CrossRenderer::Stringify ( Info.Uniforms[Iterator].Type ) );
        Result = false;
        }
    // Find missing attribute bindings
    for ( auto& Iterator : Command.ShaderBufferBindings )
        {
        int Index = FindAttributeIndexFromHandle ( Iterator.AttributeHandle );
        if ( Index == -1 )
            continue;
        Info.Attributes.erase ( Info.Attributes.begin() + Index );
        }
    for ( unsigned Iterator = 0; Iterator < Info.Attributes.size(); ++Iterator )
        {
        LOG_ERROR ( "Missing binding for attribute '%s'. Type '%s'", Info.Attributes[Iterator].Name.c_str(), CrossRenderer::Stringify ( Info.Attributes[Iterator].Type ) );
        Result = false;
        }

    // Find missing textures
    for ( auto &Iterator : Command.TextureBindings )
        {
        if ( !Iterator.BindSettings.Handle )
            {
            LOG_ERROR ( "Missing texture handle for binding '%s'.", Info.Uniforms[Iterator.UniformHandle.key()].Name.c_str() );
            Result = false;
            }
        }

    return Result;
    }
}
