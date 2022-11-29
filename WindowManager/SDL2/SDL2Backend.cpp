#include "SDL2Backend.hpp"
#include <SDL.h>
#include <SDL_syswm.h>
#include <unordered_map>
#include "../../Logger.hpp"
#include "../../VectorizedContainer.hpp"

namespace CrossRenderer
{
namespace WindowManager
{
namespace SDL2
{
typedef struct SDL2WindowInfo
    {
    SDL_Window *Window;
    glm::ivec2 Position;
    glm::uvec2 Size;
    std::string Title;
    } SDL2WindowInfo;

static VectorizedContainer <SDL2WindowInfo> Windows;
std::unordered_map <SDL_Window *, RenderWindowHandle> SDL_WindowToHandleMap;

RenderWindowHandle CreateNewWindow ( const RenderWindowDescriptor &Descriptor )
    {
    int CreationFlags = SDL_WINDOW_OPENGL;
    if ( Descriptor.Resizable )
        CreationFlags |= SDL_WINDOW_RESIZABLE;
    if ( Descriptor.Fullscreen )
        CreationFlags |= SDL_WINDOW_FULLSCREEN;
    SDL2WindowInfo NewWindow;
    NewWindow.Window = SDL_CreateWindow ( "SDL window", Descriptor.Position.x, Descriptor.Position.y, Descriptor.Size.x, Descriptor.Size.y, CreationFlags );
    if ( NewWindow.Window == nullptr )
        return RenderWindowHandle::Invalid;

    NewWindow.Size = Descriptor.Size;
    NewWindow.Position = Descriptor.Position;
    NewWindow.Title = Descriptor.Title;
    RenderWindowHandle NewHandle ( Windows.GetFreeIndex() );
    Windows[NewHandle] = NewWindow;
    SDL_WindowToHandleMap.insert ( std::pair <SDL_Window *, RenderWindowHandle> ( NewWindow.Window, NewHandle ) );
    WindowManager::WindowList.insert ( NewHandle );
    return NewHandle;
    }

bool DestroyWindow ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    SDL_DestroyWindow ( info->Window );
    return true;
    }

void SetWindowPosition ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    SDL_SetWindowPosition ( info->Window, NewPosition.x, NewPosition.y );
    info->Position = NewPosition;
    }

glm::ivec2 GetWindowPosition ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    return info->Position;

    //glm::ivec2 Position;
    //SDL_GetWindowPosition ( info->Window, &Position.x, &Position.y );
    //return Position;
    }

void SetWindowSize ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    SDL_SetWindowSize ( info->Window, NewSize.x, NewSize.y );
    info->Size = NewSize;
    }

glm::uvec2 GetWindowSize ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    return info->Size;

    //glm::uvec2 Size;
    //SDL_GetWindowSize ( info->Window, ( int * ) &Size.x, ( int * ) &Size.y );
    //return Size;
    }

void SetWindowTitle ( const RenderWindowHandle &Handle, const std::string &NewTitle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    SDL_SetWindowTitle ( info->Window, NewTitle.c_str() );
    info->Title = NewTitle;
    }

std::string GetWindowTitle ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    return info->Title;
    //return SDL_GetWindowTitle ( info->Window );
    }

bool SetWindowFullscreen ( const RenderWindowHandle &Handle, const bool NewState )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    return ( SDL_SetWindowFullscreen ( info->Window, NewState ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 ) == 0 );
    }
bool IsWindowFullscreen ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    uint32_t Flags = SDL_GetWindowFlags ( info->Window );
    return ( ( Flags & ( SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP ) ) != 0 );
    }

void ProcessEvents ( void )
    {
    // Process all SDL events
    SDL_Event Event;
    while ( SDL_PollEvent ( &Event ) )
        {
        WindowEvent NewEvent;

        switch ( Event.type )
            {
            default:
                continue;
            case SDL_MOUSEMOTION:
                {
                NewEvent.EventType = WindowEventType::MouseMoved;
                NewEvent.EventData.MouseMoved.NewPosition.x = Event.motion.x;
                NewEvent.EventData.MouseMoved.NewPosition.y = Event.motion.y;
                NewEvent.EventData.MouseMoved.Delta.x = Event.motion.xrel;
                NewEvent.EventData.MouseMoved.Delta.y = Event.motion.yrel;
                break;
                }
            case SDL_MOUSEWHEEL:
                {
                NewEvent.EventType = WindowEventType::MouseWheel;
                NewEvent.EventData.MouseWheel.Amount.x = Event.wheel.x;
                NewEvent.EventData.MouseWheel.Amount.y = Event.wheel.y;
                break;
                }
            case SDL_MOUSEBUTTONDOWN:
                {
                if ( Event.key.repeat != 0 ) continue;
                NewEvent.EventType = WindowEventType::ButtonPressed;
                NewEvent.EventData.ButtonPressed.Button = Event.button.button;
                break;
                }
            case SDL_MOUSEBUTTONUP:
                {
                NewEvent.EventType = WindowEventType::ButtonReleased;
                NewEvent.EventData.ButtonReleased.Button = Event.button.button;
                break;
                }
            case SDL_KEYDOWN:
                {
                if ( Event.key.repeat != 0 ) continue;
                NewEvent.EventType = WindowEventType::KeyPressed;
                NewEvent.EventData.KeyPressed.Key = Event.key.keysym.sym;
                NewEvent.EventData.KeyPressed.ScanCode = Event.key.keysym.scancode;
                break;
                }
            case SDL_KEYUP:
                {
                NewEvent.EventType = WindowEventType::KeyReleased;
                NewEvent.EventData.KeyReleased.Key = Event.key.keysym.sym;
                NewEvent.EventData.KeyReleased.ScanCode = Event.key.keysym.scancode;
                break;
                }
            case SDL_TEXTINPUT:
                {
                NewEvent.EventType = WindowEventType::TextInput;
                memcpy ( NewEvent.EventData.TextInput.Text, Event.text.text, 32 );
                break;
                }
            case SDL_WINDOWEVENT:
                {
                SDL_Window *window = SDL_GetWindowFromID ( Event.window.windowID );
                if ( window == nullptr )
                    continue;
                static std::pair <SDL_Window *, RenderWindowHandle> CachedHandle ( 0, RenderWindowHandle() );
                if ( window != CachedHandle.first )
                    {
                    std::unordered_map<SDL_Window *, RenderWindowHandle>::iterator HandleIterator = SDL_WindowToHandleMap.find ( window );
                    if ( HandleIterator == SDL_WindowToHandleMap.end() )
                        {
                        //                        LOG_ERROR ( "Message '%s' for unknown window handle '%X'", StringifyWindowsMessage ( msg.message ), msg.hwnd );
                        continue;
                        }
                    CachedHandle.first = HandleIterator->first;
                    CachedHandle.second = HandleIterator->second;
                    }
                NewEvent.OwnerHandle = CachedHandle.second;

                switch ( Event.window.event )
                    {
                    default:
                        continue;
                    case SDL_WINDOWEVENT_CLOSE:
                        {
                        SDL_DestroyWindow ( window );
                        NewEvent.EventType = WindowEventType::WindowClosed;
                        SDL_WindowToHandleMap.erase ( SDL_WindowToHandleMap.find ( window ) );
                        Windows.ReleaseIndex ( NewEvent.OwnerHandle );
                        WindowList.erase ( NewEvent.OwnerHandle );
                        break;
                        }

                    case SDL_WINDOWEVENT_MOVED:
                        {
                        NewEvent.EventType = WindowEventType::WindowMoved;
                        NewEvent.EventData.WindowMoved.NewPosition.x = Event.window.data1;
                        NewEvent.EventData.WindowMoved.NewPosition.y = Event.window.data2;

                        Windows[NewEvent.OwnerHandle].Position = glm::ivec2 ( Event.window.data1, Event.window.data2 );
                        break;
                        }

                    case SDL_WINDOWEVENT_RESIZED:
                        {
                        NewEvent.EventType = WindowEventType::WindowResized;
                        NewEvent.EventData.WindowResized.NewSize.x = Event.window.data1;
                        NewEvent.EventData.WindowResized.NewSize.y = Event.window.data2;

                        Windows[NewEvent.OwnerHandle].Size = glm::uvec2 ( Event.window.data1, Event.window.data2 );
                        break;
                        }
                    }
                break;
                }
            }
        SendWindowEvent ( NewEvent );
        }
    }

uint32_t GetKeyCode ( std::string KeyName )
    {
    SDL_Keycode Result = SDL_GetKeyFromName ( KeyName.c_str() );
    if ( Result == SDLK_UNKNOWN )
        LOG_ERROR ( "Unrecognized key '%s'", KeyName.c_str(), SDL_GetError() );
    return Result;
    }

uint32_t GetKeyScanCode ( const std::string KeyName )
    {
    SDL_Scancode Result = SDL_GetScancodeFromName ( KeyName.c_str() );
    if ( Result == SDL_SCANCODE_UNKNOWN )
        LOG_ERROR ( "Unrecognized key '%s'", KeyName.c_str(), SDL_GetError() );
    return Result;
    }
const char *GetKeyName ( const uint32_t KeyCode )
    {
    return SDL_GetKeyName ( KeyCode );
    }
glm::ivec2 GetMousePosition ( void )
    {
    glm::ivec2 MousePosition;
    SDL_GetMouseState ( &MousePosition.x, &MousePosition.y );
    return MousePosition;
    }
uint32_t GetMouseButtonStatus ( void )
    {
    return SDL_GetMouseState ( nullptr, nullptr );
    }

std::string GetErrorDescription ( void )
    {
    return SDL_GetError();
    }

SDL_Window *GetWindowHandle ( const RenderWindowHandle &Handle )
    {
    return Windows[Handle].Window;
    }

void *GetPlatformWindowHandle ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *WindowInformation = &Windows[Handle];
    SDL_SysWMinfo info;
    SDL_VERSION ( &info.version );

    if ( SDL_GetWindowWMInfo ( WindowInformation->Window, &info ) )
        {
        switch ( info.subsystem )
            {
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
            case SDL_SYSWM_WINDOWS:
                return info.info.win.window;
#endif
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
            case SDL_SYSWM_X11:
                return ( void* ) info.info.x11.window;
            case SDL_SYSWM_WAYLAND:
                return info.info.wl.surface;
#endif
            default:
                break;
            }

        }
    return nullptr;
    }
}
}
}
