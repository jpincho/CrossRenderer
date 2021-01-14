#pragma once
#include "../OpenGLContext.h"
#include <SDL.h>

namespace CrossRenderer
{
namespace OpenGL
{
class SDL2OpenGLContext : public OpenGLContextInterface
    {
    private:
        SDL_GLContext OpenGLContextHandle;

    public:
        bool Create ( const RendererConfiguration &NewConfiguration, const RenderWindowHandle &Handle );
        bool Destroy ( void );
        bool MakeActive ( const RenderWindowHandle &Handle );
        void SwapWindowBuffer ( const RenderWindowHandle &Handle );
    };
}
}
