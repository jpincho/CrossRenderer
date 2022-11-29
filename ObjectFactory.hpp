#pragma once
#include <CrossRendererConfig.hpp>
#include "WindowManager/WindowManagerDefinitions.hpp"
#include "RendererDefinitions.hpp"

namespace CrossRenderer
{
namespace ObjectFactory
{
void CreateNewWindowManager ( const WindowManagerBackend &Backend );
void CreateNewRenderer ( const RendererBackend &Backend );
}
}
