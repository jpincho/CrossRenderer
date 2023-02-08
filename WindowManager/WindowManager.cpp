#include "WindowManager.hpp"
#if defined (CROSS_RENDERER_GLFW3_BACKEND_SUPPORT)
#include "GLFW3/GLFW3Backend.hpp"
#endif
#include "Stringify.hpp"
#include "../Logger.hpp"
#include <unordered_set>

namespace CrossRenderer
{
namespace WindowManager
{
RenderWindowHandle ( *CreateNewWindow ) ( const RenderWindowDescriptor &Descriptor ) = 0;
bool ( *DestroyWindow ) ( const RenderWindowHandle &Handle ) = 0;
void ( *SetWindowPosition ) ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition ) = 0;
glm::ivec2 ( *GetWindowPosition ) ( const RenderWindowHandle &Handle ) = 0;
void ( *SetWindowSize ) ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize ) = 0;
glm::uvec2 ( *GetWindowSize ) ( const RenderWindowHandle &Handle ) = 0;
void ( *SetWindowTitle ) ( const RenderWindowHandle &Handle, const char *NewTitle ) = 0;
const char *( *GetWindowTitle ) ( const RenderWindowHandle &Handle ) = 0;
bool ( *SetWindowState )( const RenderWindowHandle &Handle, const WindowState NewState ) = 0;
WindowState ( *GetWindowState ) ( const RenderWindowHandle &Handle ) = 0;

void ( *SetMouseCursorState ) ( const RenderWindowHandle &Handle, const bool NewState ) = 0;
bool ( *GetMouseCursorState ) ( const RenderWindowHandle &Handle ) = 0;
glm::ivec2 ( *GetMousePosition ) ( void ) = 0;
uint32_t ( *GetMouseButtonStatus ) ( void ) = 0;
void ( *MakeGLActive ) ( const RenderWindowHandle &Handle ) = 0;
void ( *SwapGLWindowBuffer ) ( const RenderWindowHandle &Handle ) = 0;

std::unordered_set<RenderWindowHandle> WindowList;

typedef std::unordered_set <void ( * ) ( const WindowEvent & ) > TWindowEventListeners;
typedef std::unordered_set <EventListener *> TClassWindowEventListeners;
TWindowEventListeners WindowEventListeners;
TClassWindowEventListeners ClassEventListeners;

void CreateNewWindowManager ( const WindowManagerBackend &Backend )
	{
	switch ( Backend )
		{
#if defined (CROSS_RENDERER_GLFW3_BACKEND_SUPPORT)
		case WindowManagerBackend::GLFW3:
#define ATTRIB(x) WindowManager::x = WindowManager::GLFW3::x
			ATTRIB ( CreateNewWindow );
			ATTRIB ( DestroyWindow );
			ATTRIB ( SetWindowPosition );
			ATTRIB ( GetWindowPosition );
			ATTRIB ( SetWindowSize );
			ATTRIB ( GetWindowSize );
			ATTRIB ( SetWindowTitle );
			ATTRIB ( GetWindowTitle );
			ATTRIB ( SetWindowState );
			ATTRIB ( GetWindowState );
			ATTRIB ( SetMouseCursorState );
			ATTRIB ( GetMouseCursorState );
			ATTRIB ( GetMousePosition );
			ATTRIB ( GetMouseButtonStatus );
			ATTRIB ( MakeGLActive );
			ATTRIB ( SwapGLWindowBuffer );
#undef ATTRIB
			break;
#endif
		default:
			LOG_ERROR ( "Window manager backend '%s' is not built", Stringify ( Backend ) );
			return;
		}
	}

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) )
	{
	WindowEventListeners.insert ( Function );
	}

void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) )
	{
	TWindowEventListeners::iterator FindResult = WindowEventListeners.find ( Function );
	if ( FindResult != WindowEventListeners.end () )
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
