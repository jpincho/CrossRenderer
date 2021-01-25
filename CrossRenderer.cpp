#include <stdexcept>
#include "CrossRenderer.h"
#include "WindowManager/WindowManager.h"
#include "Stringify.h"
#include "ObjectFactory.h"

namespace CrossRenderer
{
static WindowManagerBackend AvailableWindowManagerBackends[] =
    {
#if defined ( CROSS_RENDERER_WINDOWS_BACKEND_SUPPORT )
    WindowManagerBackend::Windows,
#endif
#if defined ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
    WindowManagerBackend::SDL2,
#endif
    };
static RendererBackend AvailableRendererBackends[] =
    {
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
    RendererBackend::OpenGLCore,
#endif
    };

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
    if ( ( ObjectFactory::CreateNewWindowManager ( NewConfiguration.DesiredWindowBackend ) == false ) ||
            ( ObjectFactory::CreateNewRenderer ( NewConfiguration.DesiredRendererBackend ) == false ) )
        return false;

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
    AvailableBackends.assign ( AvailableRendererBackends, AvailableRendererBackends + sizeof ( AvailableRendererBackends ) / sizeof ( RendererBackend ) );
    return ( AvailableBackends.size () != 0 );
    }

RendererBackend GetDefaultRendererBackendType ( void )
    {
    return AvailableRendererBackends[0];
    }

bool GetAvailableWindowManagerBackends ( std::vector <WindowManagerBackend> &AvailableBackends )
    {
    AvailableBackends.assign ( AvailableWindowManagerBackends, AvailableWindowManagerBackends + sizeof ( AvailableWindowManagerBackends ) / sizeof ( WindowManagerBackend ) );
    return ( AvailableBackends.size() != 0 );
    }

WindowManagerBackend GetDefaultWindowManagerBackendType ( void )
    {
    return AvailableWindowManagerBackends[0];
    }
}
