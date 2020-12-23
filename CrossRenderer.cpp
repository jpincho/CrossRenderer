#include <stdexcept>
#include "CrossRenderer.h"
#include "WindowManager.h"
#include <Logger/Logger.h>
#include "Stringify.h"
#include "ObjectFactory.h"

namespace CrossRenderer
{
bool Initialize ( void )
    {
    RendererConfiguration NewConfiguration;
    NewConfiguration.InitialWindowConfigurations.push_back ( RenderWindowDescriptor() );
    NewConfiguration.DesiredRendererBackend = GetDefaultRendererBackendType();
    NewConfiguration.DesiredWindowBackend = GetDefaultWindowManagerBackendType();
    return Initialize ( NewConfiguration );
    }

bool Initialize ( const RendererConfiguration &NewConfiguration )
    {
    if ( NewConfiguration.InitialWindowConfigurations.size() == 0 )
        {
        LOG_ERROR ( "Engine must be initialized with at least one window" );
        return false;
        }
    LOG_DEBUG ( "Initializing with renderer backend '%s' and window manager backend '%s'", Stringify ( NewConfiguration.DesiredRendererBackend ), Stringify ( NewConfiguration.DesiredWindowBackend ) );
    ObjectFactory::CreateNewWindowManager ( NewConfiguration.DesiredWindowBackend );
    ObjectFactory::CreateNewRenderer ( NewConfiguration.DesiredRendererBackend );

    return InitializeRenderer ( NewConfiguration );
    }

bool Shutdown ( void )
    {
    for ( auto &WindowIterator : WindowManager::WindowList )
        {
        WindowManager::DestroyWindow ( WindowIterator );
        }
    WindowManager::WindowList.clear();
    ShutdownRenderer();
    return true;
    }

bool GetAvailableRendererBackends ( std::vector <RendererBackend> &AvailableBackends )
    {
    AvailableBackends.clear ();
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
    AvailableBackends.push_back ( RendererBackend::OpenGLCore );
#endif
    return ( AvailableBackends.size() != 0 );
    }

RendererBackend GetDefaultRendererBackendType ( void )
    {
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
    return RendererBackend::OpenGLCore;
#else
    throw std::runtime_error ( std::string ( "No rendering backend available" ) );
#endif
    }

bool GetAvailableWindowManagerBackends ( std::vector <WindowManagerBackend> &AvailableBackends )
    {
    AvailableBackends.clear ();
#if defined ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
    AvailableBackends.push_back ( WindowManagerBackend::SDL2 );
#endif
    return ( AvailableBackends.size() != 0 );
    }

WindowManagerBackend GetDefaultWindowManagerBackendType ( void )
    {
#if defined ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
    return WindowManagerBackend::SDL2;
#endif
    }

bool SanityCheckRenderCommand ( const RenderCommand &Command )
    {
    ShaderInformation Info;
    bool Result = true;
    GetShaderInformation ( Command.Shader, Info );

    // Find invalid uniform handles
    for ( auto &Iterator : Command.UniformValues )
        {
        if ( Iterator.UniformHandle == ShaderUniformHandle::invalid )
            {
            LOG_ERROR ( "Invalid uniform handle" );
            Result = false;
            break;
            }
        }

    // Find invalid attribute handles
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        if ( Iterator.AttributeHandle == ShaderAttributeHandle::invalid )
            {
            LOG_ERROR ( "Invalid attribute handle" );
            Result = false;
            break;
            }
        }

    // Find invalid texture uniform handles
    for ( auto &Iterator : Command.TextureBindings )
        {
        if ( Iterator.UniformHandle == ShaderUniformHandle::invalid )
            {
            LOG_ERROR ( "Invalid texture uniform handle" );
            Result = false;
            break;
            }
        }

    // Find duplicate uniform values
    auto FindUniformInfoByHandle = [&Info] ( ShaderUniformHandle Handle )->int
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
        int Index = FindUniformInfoByHandle ( Command.UniformValues[Iterator].UniformHandle );
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
    auto FindAttributeInfoByHandle = [&Info] ( ShaderAttributeHandle Handle )->int
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
        int Index = FindAttributeInfoByHandle ( Command.ShaderBufferBindings[Iterator].AttributeHandle );
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
    for ( auto &Iterator : Command.UniformValues )
        {
        int Index = FindUniformInfoByHandle ( Iterator.UniformHandle );
        if ( Index == -1 )
            continue;
        Info.Uniforms.erase ( Info.Uniforms.begin() + Index );
        }
    for ( auto &Iterator : Command.TextureBindings )
        {
        int Index = FindUniformInfoByHandle ( Iterator.UniformHandle );
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
    for ( auto &Iterator : Command.ShaderBufferBindings )
        {
        int Index = FindAttributeInfoByHandle ( Iterator.AttributeHandle );
        if ( Index == -1 )
            continue;
        Info.Attributes.erase ( Info.Attributes.begin() + Index );
        }
    for ( unsigned Iterator = 0; Iterator < Info.Attributes.size(); ++Iterator )
        {
        LOG_ERROR ( "Missing binding for attribute '%s'. Type '%s'", Info.Attributes[Iterator].Name.c_str(), CrossRenderer::Stringify ( Info.Attributes[Iterator].Type ) );
        Result = false;
        }
    return Result;
    }
}
