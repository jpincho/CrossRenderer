#include "ObjectFactory.h"
#include "Stringify.h"
#include "WindowManager.h"
#include "Renderer.h"
#include <Logger/Logger.h>

#if defined (CROSS_RENDERER_SDL2_BACKEND_SUPPORT)
#include "WindowManager/SDL2/SDL2Backend.h"
#endif
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT)
#include "OpenGL/OpenGLRenderer.h"
#endif

namespace CrossRenderer
{
namespace ObjectFactory
{
void CreateNewWindowManager ( const WindowManagerBackend &Backend )
    {
    switch ( Backend )
        {
#if defined (CROSS_RENDERER_SDL2_BACKEND_SUPPORT)
        case WindowManagerBackend::SDL2:
#define ATTRIB(x) WindowManager::x = WindowManager::SDL2::x
            ATTRIB ( CreateNewWindow );
            ATTRIB ( DestroyWindow );
            ATTRIB ( SetWindowPosition );
            ATTRIB ( GetWindowPosition );
            ATTRIB ( SetWindowSize );
            ATTRIB ( GetWindowSize );
            ATTRIB ( SetWindowTitle );
            ATTRIB ( GetWindowTitle );
            ATTRIB ( SetWindowFullscreen );
            ATTRIB ( IsWindowFullscreen );
            ATTRIB ( ProcessEvents );
            ATTRIB ( GetKeyCode );
            ATTRIB ( GetKeyScanCode );
            ATTRIB ( GetKeyName );
            ATTRIB ( GetMousePosition );
            ATTRIB ( GetMouseButtonStatus );
            ATTRIB ( GetErrorDescription );
#undef ATTRIB
            break;
#endif
        default:
            LOG_ERROR ( "Window manager backend '%s' is not built", Stringify ( Backend ) );
            return;
        }
    }

void CreateNewRenderer ( const RendererBackend &Backend )
    {
    switch ( Backend )
        {
#if defined (CROSS_RENDERER_OPENGL_CORE_SUPPORT)
        case RendererBackend::OpenGLCore:
#define ATTRIB(x) x = OpenGL::x
            ATTRIB ( InitializeRenderer );
            ATTRIB ( ShutdownRenderer );
            ATTRIB ( StartRenderToFramebuffer );
            ATTRIB ( DisplayFramebuffer );
            ATTRIB ( StartFrame );
            ATTRIB ( EndFrame );
            ATTRIB ( RunCommand );
            ATTRIB ( CreateShaderBuffer );
            ATTRIB ( DeleteShaderBuffer );
            ATTRIB ( ChangeShaderBufferContents );
            ATTRIB ( CreateTexture );
            ATTRIB ( DeleteTexture );
            ATTRIB ( Load2DTextureData );
            ATTRIB ( Update2DTextureRegion );
            ATTRIB ( LoadCubeMapTextureData );
            ATTRIB ( GetTextureDimensions );
            ATTRIB ( CreateShader );
            ATTRIB ( DeleteShader );
            ATTRIB ( ChangeShaderCode );
            ATTRIB ( GetShaderUniformList );
            ATTRIB ( GetShaderUniformHandle );
            ATTRIB ( GetShaderAttributeList );
            ATTRIB ( GetShaderAttributeHandle );
            ATTRIB ( GetShaderInformation );
            ATTRIB ( CreateFramebuffer );
            ATTRIB ( DeleteFramebuffer );
            ATTRIB ( GetFramebufferSize );
            ATTRIB ( GetColorBufferFromFramebuffer );
            ATTRIB ( GetDepthBufferFromFramebuffer );
#undef ATTRIB
            break;
#endif
        default:
            LOG_ERROR ( "Renderer backend '%s' is not built", Stringify ( Backend ) );
            return;
        }
    }
}
}
