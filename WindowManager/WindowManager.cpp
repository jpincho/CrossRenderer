#include "WindowManager.h"
#include <unordered_set>

namespace CrossRenderer
{
namespace WindowManager
{
RenderWindowHandle ( *CreateNewWindow ) ( const RenderWindowDescriptor &Descriptor );
bool ( *DestroyWindow ) ( const RenderWindowHandle &Handle );
void ( *SetWindowPosition ) ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition );
glm::ivec2 ( *GetWindowPosition ) ( const RenderWindowHandle &Handle );
void ( *SetWindowSize ) ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize );
glm::uvec2 ( *GetWindowSize ) ( const RenderWindowHandle &Handle );
void ( *SetWindowTitle ) ( const RenderWindowHandle &Handle, const std::string &NewTitle );
std::string ( *GetWindowTitle ) ( const RenderWindowHandle &Handle );
bool ( *SetWindowFullscreen ) ( const RenderWindowHandle &Handle, const bool NewState );
bool ( *IsWindowFullscreen ) ( const RenderWindowHandle &Handle );

void ( *ProcessEvents ) ( void );

uint32_t ( *GetKeyCode ) ( const std::string KeyName );
uint32_t ( *GetKeyScanCode ) ( const std::string KeyName );
const char * ( *GetKeyName ) ( const uint32_t KeyCode );
glm::ivec2 ( *GetMousePosition ) ( void );
uint32_t ( *GetMouseButtonStatus ) ( void );
std::string ( *GetErrorDescription ) ( void );
std::unordered_set<RenderWindowHandle> WindowList;

typedef std::unordered_set <void ( * ) ( const WindowEvent & ) > TWindowEventListeners;
typedef std::unordered_set <EventListener *> TClassWindowEventListeners;
TWindowEventListeners WindowEventListeners;
TClassWindowEventListeners ClassEventListeners;

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) )
    {
    WindowEventListeners.insert ( Function );
    }

void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) )
    {
    TWindowEventListeners::iterator FindResult = WindowEventListeners.find ( Function );
    if ( FindResult != WindowEventListeners.end() )
        WindowEventListeners.erase ( FindResult );
    }

void AddEventListener ( EventListener *Listener )
    {
    ClassEventListeners.insert ( Listener );
    }

void DeleteEventListener ( EventListener *Listener )
    {
    ClassEventListeners.erase ( Listener );
    }

void SendWindowEvent ( const WindowEvent &NewEvent )
    {
    for ( auto Iterator : WindowEventListeners )
        Iterator ( NewEvent );
    for ( auto &Iterator : ClassEventListeners )
        Iterator->OnEvent ( NewEvent );
    }
}
}
