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
}
