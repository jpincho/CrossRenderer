#pragma once
#include "../WindowManager.h"
#include <windows.h>

namespace CrossRenderer
{
namespace WindowManager
{
namespace Windows
{
RenderWindowHandle CreateNewWindow ( const RenderWindowDescriptor &Descriptor );
bool DestroyWindow ( const RenderWindowHandle &Handle );
void SetWindowPosition ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition );
glm::ivec2 GetWindowPosition ( const RenderWindowHandle &Handle );
void SetWindowSize ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize );
glm::uvec2 GetWindowSize ( const RenderWindowHandle &Handle );
void SetWindowTitle ( const RenderWindowHandle &Handle, const std::string &NewTitle );
std::string GetWindowTitle ( const RenderWindowHandle &Handle );
bool SetWindowState ( const RenderWindowHandle &Handle, const WindowState NewState );
WindowState GetWindowState ( const RenderWindowHandle &Handle );

void ProcessEvents ( void );

uint32_t GetKeyCode ( const std::string KeyName );
uint32_t GetKeyScanCode ( const std::string KeyName );
const char *GetKeyName ( const uint32_t KeyCode );
glm::ivec2 GetMousePosition ( void );
uint32_t GetMouseButtonStatus ( void );
std::string GetErrorDescription ( void );
HWND GetWindowHandle ( const RenderWindowHandle &Handle );

WindowManagerBackend GetBackendType ( void );
}
}
}
