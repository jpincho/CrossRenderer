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
extern void ( *SetWindowTitle ) ( const RenderWindowHandle &Handle, const std::string &NewTitle );
extern std::string ( *GetWindowTitle ) ( const RenderWindowHandle &Handle );
extern bool ( *SetWindowFullscreen ) ( const RenderWindowHandle &Handle, const bool NewState );
extern bool ( *IsWindowFullscreen ) ( const RenderWindowHandle &Handle );

extern void ( *ProcessEvents ) ( void );

extern uint32_t ( *GetKeyCode ) ( const std::string KeyName );
extern uint32_t ( *GetKeyScanCode ) ( const std::string KeyName );
extern const char * ( *GetKeyName ) ( const uint32_t KeyCode );
extern glm::ivec2 ( *GetMousePosition ) ( void );
extern uint32_t ( *GetMouseButtonStatus ) ( void );
extern std::string ( *GetErrorDescription ) ( void );
extern std::unordered_set<RenderWindowHandle> WindowList;

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) );
void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) );
void AddEventListener ( EventListener *Listener );
void DeleteEventListener ( EventListener *Listener );
void SendWindowEvent ( const WindowEvent &NewEvent );
}
}
