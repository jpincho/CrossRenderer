#include "WindowManager.hpp"
#include <CrossRendererConfig.hpp>
#if defined (CROSS_RENDERER_GLFW3_BACKEND_SUPPORT)
#include "GLFW3/GLFW3Backend.hpp"
#endif
#include "Stringify.hpp"
#include "../Internal/Logger.hpp"
#include <unordered_set>

namespace CrossRenderer
{
namespace WindowManager
{
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return (*name) ( __VA_ARGS__ ) = 0

DECLARE_INTERFACE_FUNCTION ( RenderWindowHandle, CreateNewWindow, const RenderWindowDescriptor &Descriptor );
DECLARE_INTERFACE_FUNCTION ( bool, DestroyWindow, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetWindowPosition, const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition );
DECLARE_INTERFACE_FUNCTION ( glm::ivec2, GetWindowPosition, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetWindowSize, const RenderWindowHandle &Handle, const glm::uvec2 &NewSize );
DECLARE_INTERFACE_FUNCTION ( glm::uvec2, GetWindowSize, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetWindowTitle, const RenderWindowHandle &Handle, const char *NewTitle );
DECLARE_INTERFACE_FUNCTION ( const char *, GetWindowTitle, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( bool, SetWindowState, const RenderWindowHandle &Handle, const WindowState NewState );
DECLARE_INTERFACE_FUNCTION ( WindowState, GetWindowState, const RenderWindowHandle &Handle );

DECLARE_INTERFACE_FUNCTION ( void, SetMouseCursorState, const RenderWindowHandle &Handle, const bool NewState );
DECLARE_INTERFACE_FUNCTION ( bool, GetMouseCursorState, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( glm::ivec2, GetMousePosition, void );
DECLARE_INTERFACE_FUNCTION ( uint32_t, GetMouseButtonStatus, void );
DECLARE_INTERFACE_FUNCTION ( void, MakeGLActive, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SwapGLWindowBuffer, const RenderWindowHandle &Handle );

#undef DECLARE_INTERFACE_FUNCTION

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

void CreateNewWindowManager ( const WindowManager::WindowManagerBackend &Backend )
	{
	switch ( Backend )
		{
#if defined (CROSS_RENDERER_GLFW3_BACKEND_SUPPORT)
		case WindowManager::WindowManagerBackend::GLFW3:
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
}
