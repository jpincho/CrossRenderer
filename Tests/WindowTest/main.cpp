#include <Logger/Logger.h>
#include "../Stringify.h"
#include "../CrossRenderer.h"
#include "../WindowManager.h"

void Function ( const CrossRenderer::WindowEvent &Event )
    {
    LOG_DEBUG ( "%s", CrossRenderer::Stringify ( Event.EventType ) );
    }

int main ( void )
    {
    std::vector <CrossRenderer::WindowManagerBackend> AvailableWindowManagerBackends;
    std::vector <CrossRenderer::RendererBackend> AvailableRendererBackends;

    CrossRenderer::GetAvailableWindowManagerBackends ( AvailableWindowManagerBackends );
    CrossRenderer::GetAvailableRendererBackends ( AvailableRendererBackends );
    for ( auto RendererBackendIterator : AvailableRendererBackends )
        {
        for ( auto WindowManagerBackendIterator : AvailableWindowManagerBackends )
            {
            CrossRenderer::RendererConfiguration NewConfiguration;
            NewConfiguration.InitialWindowConfigurations.push_back ( CrossRenderer::RenderWindowDescriptor() );
            NewConfiguration.InitialWindowConfigurations.push_back ( CrossRenderer::RenderWindowDescriptor() );
            NewConfiguration.DesiredRendererBackend = RendererBackendIterator;
            NewConfiguration.DesiredWindowBackend = WindowManagerBackendIterator;
            if ( CrossRenderer::Initialize ( NewConfiguration ) == false )
                return -1;
            for ( const auto &WindowHandle : CrossRenderer::WindowManager::WindowList )
                {
                CrossRenderer::WindowManager::SetWindowTitle ( WindowHandle, std::string ( "CrossRenderer - " ) + CrossRenderer::Stringify ( RendererBackendIterator ) + std::string ( " - " ) + CrossRenderer::Stringify ( WindowManagerBackendIterator ) );
                }

            CrossRenderer::WindowManager::AddEventListener ( Function );

            while ( CrossRenderer::WindowManager::WindowList.size() )
                {
                CrossRenderer::WindowManager::ProcessEvents();
                }
            CrossRenderer::Shutdown();
            }
        }
    return 0;
    }
