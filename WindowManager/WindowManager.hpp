#pragma once
#include "WindowManagerDefinitions.hpp"
#include <unordered_set>

namespace CrossRenderer
{
namespace WindowManager
{
// Tricking intellisense into thinking these are actual functions. Very useful
#if defined (__INTELLISENSE__)
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return name ( __VA_ARGS__ )
#else
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
extern return (*name) ( __VA_ARGS__ )
#endif

DECLARE_INTERFACE_FUNCTION ( RenderWindowHandle, CreateNewWindow, const RenderWindowDescriptor &Descriptor );
DECLARE_INTERFACE_FUNCTION ( bool, DestroyWindow, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetWindowPosition, const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition );
DECLARE_INTERFACE_FUNCTION ( glm::ivec2, GetWindowPosition, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetWindowSize, const RenderWindowHandle &Handle, const glm::uvec2 &NewSize );
DECLARE_INTERFACE_FUNCTION ( glm::uvec2, GetWindowSize, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( glm::uvec2, GetWindowFramebufferSize, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetWindowTitle, const RenderWindowHandle &Handle, const char *NewTitle );
DECLARE_INTERFACE_FUNCTION ( const char *, GetWindowTitle, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( bool, SetWindowState, const RenderWindowHandle &Handle, const WindowState NewState );
DECLARE_INTERFACE_FUNCTION ( WindowState, GetWindowState, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, ProcessMessages, void );

DECLARE_INTERFACE_FUNCTION ( void, SetMouseCursorState, const RenderWindowHandle &Handle, const bool NewState );
DECLARE_INTERFACE_FUNCTION ( bool, GetMouseCursorState, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( glm::ivec2, GetMousePosition, void );
DECLARE_INTERFACE_FUNCTION ( uint32_t, GetMouseButtonStatus, void );
DECLARE_INTERFACE_FUNCTION ( void, MakeGLActive, const RenderWindowHandle &Handle );
DECLARE_INTERFACE_FUNCTION ( void, SwapGLWindowBuffer, const RenderWindowHandle &Handle );
#undef DECLARE_INTERFACE_FUNCTION

extern std::unordered_set<RenderWindowHandle> WindowList;

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) );
void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) );
void AddEventListener ( EventListener *Listener );
void DeleteEventListener ( EventListener *Listener );
void SendWindowEvent ( const WindowEvent &NewEvent );
}
void CreateNewWindowManager ( const WindowManager::WindowManagerBackend &Backend );
}
