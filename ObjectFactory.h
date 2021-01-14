#pragma once
#include <CrossRendererConfig.h>
#include "WindowManager/WindowManagerDefinitions.h"
#include "RendererDefinitions.h"

namespace CrossRenderer
{
namespace ObjectFactory
{
void CreateNewWindowManager ( const WindowManagerBackend &Backend );
void CreateNewRenderer ( const RendererBackend &Backend );
}
}
