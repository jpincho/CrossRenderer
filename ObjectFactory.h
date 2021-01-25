#pragma once
#include <CrossRendererConfig.h>
#include "WindowManager/WindowManagerDefinitions.h"
#include "RendererDefinitions.h"

namespace CrossRenderer
{
namespace ObjectFactory
{
bool CreateNewWindowManager ( const WindowManagerBackend &Backend );
bool CreateNewRenderer ( const RendererBackend &Backend );
}
}
