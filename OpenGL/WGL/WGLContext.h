#pragma once
#include <windows.h>
#include "../OpenGLContext.h"

namespace CrossRenderer
{
namespace OpenGL
{
class WGLContext : public OpenGLContextInterface
    {
    private:
        HGLRC GLContext;

    public:
        bool Create ( const RendererConfiguration &NewConfiguration, const RenderWindowHandle &Handle );
        bool Destroy ( void );
        bool MakeActive ( const RenderWindowHandle &Handle );
        void SwapWindowBuffer ( const RenderWindowHandle &Handle );
    };
}
}
