#pragma once
#include <CrossRendererConfig.hpp>
#include "WindowManager/WindowManager.hpp"
#include "Renderer/Renderer.hpp"

namespace CrossRenderer
{
bool Initialize ( void );
bool Initialize ( const RendererConfiguration &NewConfiguration );
bool Shutdown ( void );
bool GetAvailableRendererBackends ( std::vector <RendererBackend> &AvailableBackends );
RendererBackend GetDefaultRendererBackendType ( void );
bool GetAvailableWindowManagerBackends ( std::vector <WindowManagerBackend> &AvailableBackends );
WindowManagerBackend GetDefaultWindowManagerBackendType ( void );
}
