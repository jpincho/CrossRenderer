#pragma once
#include <CrossRendererConfig.hpp>
#include "WindowManagerDefinitions.hpp"
#include <unordered_set>

namespace CrossRenderer
{
namespace WindowManager
{
extern RenderWindowHandle ( *CreateNewWindow ) ( const RenderWindowDescriptor &Descriptor );
extern bool ( *DestroyWindow ) ( const RenderWindowHandle &Handle );
extern void ( *SetWindowPosition ) ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition );
extern glm::ivec2 ( *GetWindowPosition ) ( const RenderWindowHandle &Handle );
extern void ( *SetWindowSize ) ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize );
extern glm::uvec2 ( *GetWindowSize ) ( const RenderWindowHandle &Handle );
extern void ( *SetWindowTitle ) ( const RenderWindowHandle &Handle, const char *NewTitle );
extern const char *( *GetWindowTitle ) ( const RenderWindowHandle &Handle );
extern bool (*SetWindowState )( const RenderWindowHandle &Handle, const WindowState NewState );
extern WindowState (*GetWindowState) ( const RenderWindowHandle &Handle );

extern void (*SetMouseCursorState) ( const RenderWindowHandle &Handle, const bool NewState );
extern bool (*GetMouseCursorState) ( const RenderWindowHandle &Handle );
extern glm::ivec2 ( *GetMousePosition ) ( void );
extern uint32_t ( *GetMouseButtonStatus ) ( void );
extern void (*MakeGLActive) ( const RenderWindowHandle &Handle );
extern void (*SwapGLWindowBuffer) ( const RenderWindowHandle &Handle );

extern std::unordered_set<RenderWindowHandle> WindowList;
void CreateNewWindowManager ( const WindowManagerBackend &Backend );

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) );
void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) );
void AddEventListener ( EventListener *Listener );
void DeleteEventListener ( EventListener *Listener );
void SendWindowEvent ( const WindowEvent &NewEvent );
}
}
