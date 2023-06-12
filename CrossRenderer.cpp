#include <CrossRendererConfig.hpp>
#include "CrossRenderer.hpp"
#include "Internal/Logger.hpp"
#include "WindowManager/WindowManager.hpp"
#include "Renderer/Stringify.hpp"
#include "WindowManager/Stringify.hpp"
#include <stdexcept>

namespace CrossRenderer
{
bool Initialize ( void )
    {
    RendererConfiguration NewConfiguration;
    NewConfiguration.InitialWindowConfigurations.push_back ( WindowManager::RenderWindowDescriptor() );
    NewConfiguration.DesiredRendererBackend = GetDefaultRendererBackendType();
    NewConfiguration.DesiredWindowBackend = GetDefaultWindowManagerBackendType();
    return Initialize ( NewConfiguration );
    }

bool Initialize ( RendererConfiguration &NewConfiguration )
    {
    if ( NewConfiguration.InitialWindowConfigurations.size() == 0 )
        {
        LOG_WARNING ( "Engine must be initialized with at least one window. Adding a default one" );
		NewConfiguration.InitialWindowConfigurations.push_back ( WindowManager::RenderWindowDescriptor() );
        }
    LOG_DEBUG ( "Initializing with renderer backend '%s' and window manager backend '%s'", Stringify ( NewConfiguration.DesiredRendererBackend ), Stringify ( NewConfiguration.DesiredWindowBackend ) );
    CreateNewWindowManager ( NewConfiguration.DesiredWindowBackend );
    CreateNewRenderer ( NewConfiguration.DesiredRendererBackend );

    return InitializeRenderer ( NewConfiguration );
    }

bool Shutdown ( void )
    {
	while ( WindowManager::WindowList.size()>0 )
        {
        WindowManager::DestroyWindow ( *WindowManager::WindowList.begin() );
        }
	assert ( WindowManager::WindowList.size () == 0 );
    ShutdownRenderer();
    return true;
    }

bool GetAvailableRendererBackends ( std::vector <RendererBackend> &AvailableBackends )
    {
    AvailableBackends.clear ();
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
    AvailableBackends.push_back ( RendererBackend::OpenGLCore );
#endif
#if defined ( CROSS_RENDERER_DIRECT3D11_SUPPORT )
	AvailableBackends.push_back ( RendererBackend::Direct3D11 );
#endif
    return ( AvailableBackends.size() != 0 );
    }

RendererBackend GetDefaultRendererBackendType ( void )
    {
#if defined ( CROSS_RENDERER_DIRECT3D11_SUPPORT )
	return RendererBackend::Direct3D11;
#elif defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
    return RendererBackend::OpenGLCore;
#else
    throw std::runtime_error ( std::string ( "No rendering backend available" ) );
#endif
    }

bool GetAvailableWindowManagerBackends ( std::vector <WindowManager::WindowManagerBackend> &AvailableBackends )
    {
    AvailableBackends.clear ();
#if defined ( CROSS_RENDERER_GLFW3_BACKEND_SUPPORT )
    AvailableBackends.push_back ( WindowManager::WindowManagerBackend::GLFW3 );
#endif
	return ( AvailableBackends.size() != 0 );
    }

WindowManager::WindowManagerBackend GetDefaultWindowManagerBackendType ( void )
    {
#if defined ( CROSS_RENDERER_GLFW3_BACKEND_SUPPORT )
    return WindowManager::WindowManagerBackend::GLFW3;
#endif
	}
}
