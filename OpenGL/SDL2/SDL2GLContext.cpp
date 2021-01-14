#include "SDL2GLContext.h"
#include "../../WindowManager/SDL2/SDL2Backend.h"
#include <glad/glad.h>
#include <Logger/Logger.h>

namespace CrossRenderer
{
namespace OpenGL
{
bool SDL2OpenGLContext::Create ( const RendererConfiguration &NewConfiguration, const RenderWindowHandle &Handle )
    {
    SDL_GL_SetAttribute ( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute ( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
    SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute ( SDL_GL_RED_SIZE, NewConfiguration.RedBits );
    SDL_GL_SetAttribute ( SDL_GL_GREEN_SIZE, NewConfiguration.GreenBits );
    SDL_GL_SetAttribute ( SDL_GL_BLUE_SIZE, NewConfiguration.BlueBits );
    SDL_GL_SetAttribute ( SDL_GL_ALPHA_SIZE, NewConfiguration.AlphaBits );
    SDL_GL_SetAttribute ( SDL_GL_ACCELERATED_VISUAL, 1 );

    SDL_Window *SDLWindowHandle = WindowManager::SDL2::GetWindowHandle ( Handle );

    Version SupportedVersion[] = {Version ( 4, 6 ), Version ( 4, 5 ), Version ( 4, 4 ), Version ( 4, 3 ), Version ( 4, 2 ), Version ( 4, 1 ), Version ( 4, 0 ), Version ( 3, 2 ), Version ( 3, 1 ), Version ( 3, 0 ) };
    for ( unsigned Iterator = 0; Iterator < sizeof ( SupportedVersion ) / sizeof ( Version ); ++Iterator )
        {
        SDL_GL_SetAttribute ( SDL_GL_CONTEXT_MAJOR_VERSION, SupportedVersion[Iterator].Major );
        SDL_GL_SetAttribute ( SDL_GL_CONTEXT_MINOR_VERSION, SupportedVersion[Iterator].Minor );
        OpenGLContextHandle = SDL_GL_CreateContext ( SDLWindowHandle );
        if ( OpenGLContextHandle != nullptr ) break;
        }

    if ( OpenGLContextHandle == nullptr )
        {
        LOG_ERROR ( "Unable to create opengl context. '%s'", SDL_GetError() );
        return false;
        }

    if ( SDL_GL_MakeCurrent ( SDLWindowHandle, OpenGLContextHandle ) != 0 )
        {
        LOG_ERROR ( "Unable to activate opengl context. '%s'", SDL_GetError() );
        SDL_GL_DeleteContext ( OpenGLContextHandle );
        return false;
        }

    if ( SDL_GL_SetSwapInterval ( NewConfiguration.VSyncEnabled ) != 0 )
        LOG_ERROR ( "Swap interval setup failed" );

    if ( !gladLoadGLLoader ( ( GLADloadproc ) SDL_GL_GetProcAddress ) )
        return false;

    DetectOpenGLVersion();
    DetectGLSLVersion();
    DetectOpenGLExtensions();
    return true;
    }

bool SDL2OpenGLContext::Destroy ( void )
    {
    SDL_GL_MakeCurrent ( nullptr, nullptr );
    SDL_GL_DeleteContext ( OpenGLContextHandle );
    OpenGLContextHandle = nullptr;
    return true;
    }

bool SDL2OpenGLContext::MakeActive ( const RenderWindowHandle &Handle )
    {
    SDL_Window *SDLWindowHandle = WindowManager::SDL2::GetWindowHandle ( Handle );
    return SDL_GL_MakeCurrent ( SDLWindowHandle, OpenGLContextHandle ) == 0;
    }

void SDL2OpenGLContext::SwapWindowBuffer ( const RenderWindowHandle &Handle )
    {
    SDL_Window *SDLWindowHandle = WindowManager::SDL2::GetWindowHandle ( Handle );
    SDL_GL_SwapWindow ( SDLWindowHandle );
    }
}
}
