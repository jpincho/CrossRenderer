#include <stdexcept>
#include "CrossRenderer.h"
#include "WindowManager/WindowManager.h"
#include <Logger/Logger.h>
#include "Stringify.h"
#include "ObjectFactory.h"

namespace CrossRenderer
{
static std::vector <WindowManagerBackend> AvailableWindowManagerBackends;
static std::vector <RendererBackend> AvailableRendererBackends;

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
    if ( AvailableRendererBackends.size() == 0 )
        {
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
        AvailableRendererBackends.push_back ( RendererBackend::OpenGLCore );
#endif
        }
    AvailableBackends = AvailableRendererBackends;
    return ( AvailableRendererBackends.size() != 0 );
    }

RendererBackend GetDefaultRendererBackendType ( void )
    {
    if ( ( AvailableRendererBackends.size() == 0 ) && ( GetAvailableRendererBackends ( AvailableRendererBackends ) == false ) )
        throw std::runtime_error ( std::string ( "No rendering backend available" ) );
    return AvailableRendererBackends[0];
    }

bool GetAvailableWindowManagerBackends ( std::vector <WindowManagerBackend> &AvailableBackends )
    {
    if ( AvailableWindowManagerBackends.size() == 0 )
        {
#if defined ( CROSS_RENDERER_WINDOWS_BACKEND_SUPPORT )
        AvailableWindowManagerBackends.push_back ( WindowManagerBackend::Windows );
#endif
#if defined ( CROSS_RENDERER_SDL2_BACKEND_SUPPORT )
        AvailableWindowManagerBackends.push_back ( WindowManagerBackend::SDL2 );
#endif
        }
    AvailableBackends = AvailableWindowManagerBackends;
    return ( AvailableWindowManagerBackends.size() != 0 );
    }

WindowManagerBackend GetDefaultWindowManagerBackendType ( void )
    {
    if ( ( AvailableWindowManagerBackends.size() == 0 ) && ( GetAvailableWindowManagerBackends ( AvailableWindowManagerBackends ) == false ) )
        throw std::runtime_error ( std::string ( "No window manager backend available" ) );
    return AvailableWindowManagerBackends[0];
    }
}
