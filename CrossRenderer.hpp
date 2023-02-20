#pragma once
#include "WindowManager/WindowManager.hpp"
#include "Renderer/Renderer.hpp"

namespace CrossRenderer
{
bool Initialize ( void );
bool Initialize ( RendererConfiguration &NewConfiguration );
bool Shutdown ( void );
bool GetAvailableRendererBackends ( std::vector <RendererBackend> &AvailableBackends );
RendererBackend GetDefaultRendererBackendType ( void );
bool GetAvailableWindowManagerBackends ( std::vector <WindowManager::WindowManagerBackend> &AvailableBackends );
WindowManager::WindowManagerBackend GetDefaultWindowManagerBackendType ( void );
}
