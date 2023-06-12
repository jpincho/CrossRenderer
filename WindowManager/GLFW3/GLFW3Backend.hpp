#pragma once
#include "../WindowManager.hpp"

namespace CrossRenderer
{
namespace WindowManager
{
namespace GLFW3
{
RenderWindowHandle CreateNewWindow ( const RenderWindowDescriptor &Descriptor );
bool DestroyWindow ( const RenderWindowHandle &Handle );
void SetWindowPosition ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition );
glm::ivec2 GetWindowPosition ( const RenderWindowHandle &Handle );
void SetWindowSize ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize );
glm::uvec2 GetWindowSize ( const RenderWindowHandle &Handle );
void SetWindowTitle ( const RenderWindowHandle &Handle, const char *NewTitle );
const char *GetWindowTitle ( const RenderWindowHandle &Handle );
bool SetWindowState ( const RenderWindowHandle &Handle, const WindowState NewState );
WindowState GetWindowState ( const RenderWindowHandle &Handle );

void SetMouseCursorState ( const RenderWindowHandle &Handle, const bool NewState );
bool GetMouseCursorState ( const RenderWindowHandle &Handle );
glm::ivec2 GetMousePosition ( void );
uint32_t GetMouseButtonStatus ( void );

void *GetPlatformWindowHandle ( const RenderWindowHandle &Handle );
void MakeGLActive ( const RenderWindowHandle &Handle );
void SwapGLWindowBuffer ( const RenderWindowHandle &Handle );
}
}
}
