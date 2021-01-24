#include <Logger/Logger.h>
#include "../Stringify.h"
#include "../CrossRenderer.h"
#include "../WindowManager/WindowManager.h"

void Function ( const CrossRenderer::WindowEvent& Event )
    {
    switch ( Event.EventType )
        {
        default:
            LOG_DEBUG ( "%s", CrossRenderer::Stringify ( Event.EventType ) );
            break;
        case CrossRenderer::WindowEventType::KeyReleased:
            {
            LOG_DEBUG ( "Key released - %s", CrossRenderer::WindowManager::GetKeyName ( Event.EventData.KeyReleased.Key ) );
            if ( Event.EventData.KeyReleased.Key == '1' )
                {
                //bool FullScreen = CrossRenderer::WindowManager::IsWindowFullscreen ( *CrossRenderer::WindowManager::WindowList.begin() );
                LOG_DEBUG ( "State = %s", CrossRenderer::Stringify ( CrossRenderer::WindowManager::GetWindowState ( *CrossRenderer::WindowManager::WindowList.begin() ) ) );
                CrossRenderer::WindowManager::SetWindowState ( *CrossRenderer::WindowManager::WindowList.begin(), CrossRenderer::WindowState::Minimized );
                }
            if ( Event.EventData.KeyReleased.Key == '2' )
                {
                //bool FullScreen = CrossRenderer::WindowManager::IsWindowFullscreen ( *CrossRenderer::WindowManager::WindowList.begin() );
                LOG_DEBUG ( "State = %s", CrossRenderer::Stringify ( CrossRenderer::WindowManager::GetWindowState ( *CrossRenderer::WindowManager::WindowList.begin () ) ) );
                CrossRenderer::WindowManager::SetWindowState ( *CrossRenderer::WindowManager::WindowList.begin (), CrossRenderer::WindowState::Normal );
                }
            if ( Event.EventData.KeyReleased.Key == '3' )
                {
                //bool FullScreen = CrossRenderer::WindowManager::IsWindowFullscreen ( *CrossRenderer::WindowManager::WindowList.begin() );
                LOG_DEBUG ( "State = %s", CrossRenderer::Stringify ( CrossRenderer::WindowManager::GetWindowState ( *CrossRenderer::WindowManager::WindowList.begin () ) ) );
                CrossRenderer::WindowManager::SetWindowState ( *CrossRenderer::WindowManager::WindowList.begin (), CrossRenderer::WindowState::Maximized );
                }
            break;
            }
        case CrossRenderer::WindowEventType::ButtonPressed:
            {
            char* Buttons[] = { "Left", "Right", "Middle" };
            LOG_DEBUG ( "Button pressed - %s", Buttons[Event.EventData.ButtonPressed.Button] );
            break;
            }
        case CrossRenderer::WindowEventType::ButtonReleased:
            {
            char* Buttons[] = { "Left", "Right", "Middle" };
            LOG_DEBUG ( "Button released - %s", Buttons[Event.EventData.ButtonReleased.Button] );
            break;
            }
        case CrossRenderer::WindowEventType::MouseWheel:
            {
            LOG_DEBUG ( "Button wheel - %i %i", Event.EventData.MouseWheel.X, Event.EventData.MouseWheel.Y );
            break;
            }
        }
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
