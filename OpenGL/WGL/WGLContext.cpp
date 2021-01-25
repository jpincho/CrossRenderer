#include "WGLContext.h"
#include "../../WindowManager/Windows/WindowsBackend.h"
#include <glad/glad.h>
#include <glad/glad_wgl.h>

namespace CrossRenderer
{
namespace OpenGL
{
void *WGLGetProcAddress ( const char *Function )
    {
    PROC Result = wglGetProcAddress ( Function );
    if ( Result )
        return ( void* ) Result;

    static HINSTANCE ModuleInstance = NULL;
    if ( ModuleInstance == NULL )
        {
        ModuleInstance = LoadLibrary ( "OpenGL32.dll" );
        }
    return ( void* ) GetProcAddress ( ModuleInstance, Function );
    }

bool WGLContext::Create ( const RendererConfiguration &NewConfiguration, const RenderWindowHandle &Handle )
    {
    WNDCLASSEX WindowClass;
    if ( !GetClassInfoEx ( GetModuleHandle ( nullptr ), "CrossRendererFakeWindowClass", &WindowClass ) )
        {
        memset ( &WindowClass, 0, sizeof ( WNDCLASSEX ) );
        WindowClass.cbSize = sizeof ( WNDCLASSEX );
        WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        WindowClass.lpfnWndProc = DefWindowProc;
        WindowClass.cbClsExtra = 0;
        WindowClass.cbWndExtra = 0;
        WindowClass.hInstance = GetModuleHandle ( nullptr );
        WindowClass.hIcon = LoadIcon ( nullptr, IDI_APPLICATION );
        WindowClass.hCursor = LoadCursor ( nullptr, IDC_ARROW );
        WindowClass.hbrBackground = ( HBRUSH ) GetStockObject ( BLACK_BRUSH );
        WindowClass.lpszMenuName = nullptr;
        WindowClass.lpszClassName = "CrossRendererFakeWindowClass";
        WindowClass.hIconSm = LoadIcon ( nullptr, IDI_WINLOGO );

        if ( !RegisterClassEx ( &WindowClass ) )
            {
            /**
             * Either an OS Error or a window with the same "name" id will cause
             * this to fail:
             */
            return RenderWindowHandle::invalid;
            }
        }

    // Create an OpenGL 1.1 context
    HWND fakeWND = CreateWindow (
                       "CrossRendererFakeWindowClass", "Fake Window",      // window class, title
                       WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // style
                       0, 0,                       // position x, y
                       1, 1,                       // width, height
                       NULL, NULL,                 // parent window, menu
                       NULL, NULL );          // instance, param

    HDC fakeDC = GetDC ( fakeWND );

    PIXELFORMATDESCRIPTOR fakePFD;
    ZeroMemory ( &fakePFD, sizeof ( fakePFD ) );
    fakePFD.nSize = sizeof ( fakePFD );
    fakePFD.nVersion = 1;
    fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fakePFD.iPixelType = PFD_TYPE_RGBA;
    fakePFD.cColorBits = 32;
    fakePFD.cAlphaBits = 8;
    fakePFD.cDepthBits = 24;

    int fakePFDID = ChoosePixelFormat ( fakeDC, &fakePFD );
    if ( fakePFDID == 0 )
        return false;

    if ( SetPixelFormat ( fakeDC, fakePFDID, &fakePFD ) == false )
        return false;

    HGLRC fakeRC = wglCreateContext ( fakeDC );

    if ( fakeRC == 0 )
        return false;
    if ( wglMakeCurrent ( fakeDC, fakeRC ) == false )
        return false;

    if ( !gladLoadWGL ( fakeDC ) )
        return false;

    // Now that the fake context is created and I have the WGL extensions, create a decent context
    HWND WindowHandle = WindowManager::Windows::GetWindowHandle ( Handle );
    HDC WindowDC = GetWindowDC ( WindowHandle );
    const int pixelAttribs[] =
        {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, NewConfiguration.RedBits + NewConfiguration.GreenBits + NewConfiguration.BlueBits,
        WGL_ALPHA_BITS_ARB, NewConfiguration.AlphaBits,
        WGL_DEPTH_BITS_ARB, NewConfiguration.DepthBits,
        WGL_STENCIL_BITS_ARB, NewConfiguration.StencilBits,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
        };

    int pixelFormatID;
    UINT numFormats;
    bool status = wglChoosePixelFormatARB ( WindowDC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats );

    if ( status == false || numFormats == 0 )
        return false;
    PIXELFORMATDESCRIPTOR PFD;
    DescribePixelFormat ( WindowDC, pixelFormatID, sizeof ( PFD ), &PFD );
    SetPixelFormat ( WindowDC, pixelFormatID, &PFD );

//    const int major_min = 3, minor_min = 0;
    int  contextAttribs[] =
        {
//        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
//        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
        };

    GLContext = wglCreateContextAttribsARB ( WindowDC, 0, contextAttribs );
    if ( GLContext == NULL )
        return false;

    // Activate the new context
    if ( !wglMakeCurrent ( WindowDC, GLContext ) )
        return false;

    // Delete the old fake context, and window
    wglDeleteContext ( fakeRC );
    ReleaseDC ( fakeWND, fakeDC );
    DestroyWindow ( fakeWND );

    if ( !gladLoadWGL ( WindowDC ) )
        return false;
    if ( !gladLoadGLLoader ( ( GLADloadproc ) WGLGetProcAddress ) )
        return false;
    ShowWindow ( WindowHandle, SW_SHOW );
    if ( ( GLAD_WGL_EXT_swap_control ) && ( NewConfiguration.VSyncEnabled ) )
        {
        wglSwapIntervalEXT ( 1 );
        }
    DetectOpenGLVersion();
    DetectGLSLVersion();
    DetectOpenGLExtensions();
    return true;
    }

bool WGLContext::Destroy ( void )
    {
    wglMakeCurrent ( nullptr, GLContext );
    wglDeleteContext ( GLContext );
    GLContext = nullptr;
    return true;
    }

bool WGLContext::MakeActive ( const RenderWindowHandle &Handle )
    {
    HWND WindowHandle = WindowManager::Windows::GetWindowHandle ( Handle );
    HDC WindowDC = GetWindowDC ( WindowHandle );
    wglMakeCurrent ( WindowDC, GLContext );
    return true;
    }

void WGLContext::SwapWindowBuffer ( const RenderWindowHandle &Handle )
    {
    HWND WindowHandle = WindowManager::Windows::GetWindowHandle ( Handle );
    HDC WindowDC = GetWindowDC ( WindowHandle );
    wglSwapLayerBuffers ( WindowDC, WGL_SWAP_MAIN_PLANE );
    }
}
}
