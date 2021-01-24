#include "SDL2Backend.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <unordered_map>
#include "../../Logger.h"

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

static VectorizedContainer <SDL2WindowInfo, RenderWindowTag> Windows;
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
        return RenderWindowHandle::invalid;

    NewWindow.Size = Descriptor.Size;
    NewWindow.Position = Descriptor.Position;
    NewWindow.Title = Descriptor.Title;
    RenderWindowHandle NewHandle = Windows.GetNewHandle();
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

bool SetWindowState ( const RenderWindowHandle &Handle, const WindowState NewState )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    switch ( NewState )
        {
        case WindowState::Minimized:
            SDL_MinimizeWindow ( info->Window );
            break;
        case WindowState::Normal:
            SDL_RestoreWindow ( info->Window );
            break;
        case WindowState::Maximized:
            SDL_MaximizeWindow ( info->Window );
            break;
        }
    return true;
    }

WindowState GetWindowState ( const RenderWindowHandle &Handle )
    {
    SDL2WindowInfo *info = &Windows[Handle];
    uint32_t Flags = SDL_GetWindowFlags ( info->Window );
    if ( Flags & SDL_WINDOW_MAXIMIZED )
        return WindowState::Maximized;
    if ( Flags & SDL_WINDOW_MINIMIZED )
        return WindowState::Minimized;
    return WindowState::Normal;
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
                NewEvent.EventData.MouseMoved.X = Event.motion.x;
                NewEvent.EventData.MouseMoved.Y = Event.motion.y;
                break;
                }
            case SDL_MOUSEWHEEL:
                {
                NewEvent.EventType = WindowEventType::MouseWheel;
                NewEvent.EventData.MouseWheel.X = Event.wheel.x;
                NewEvent.EventData.MouseWheel.Y = Event.wheel.y;
                break;
                }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                if ( Event.key.repeat != 0 ) continue;
                NewEvent.EventType = ( Event.type == SDL_MOUSEBUTTONDOWN ? WindowEventType::ButtonPressed : WindowEventType::ButtonReleased );
                NewEvent.EventData.ButtonPressed.Button = Event.button.button;
                break;
                }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                {
                if ( Event.key.repeat != 0 ) continue;
                NewEvent.EventType = ( Event.type == SDL_KEYDOWN ? WindowEventType::KeyPressed : WindowEventType::KeyReleased );
                NewEvent.EventData.Raw.Data1 = Event.key.keysym.sym;
                NewEvent.EventData.Raw.Data2 = Event.key.keysym.scancode;
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
                        Windows.ReleaseHandle ( NewEvent.OwnerHandle );
                        WindowList.erase ( NewEvent.OwnerHandle );
                        break;
                        }

                    case SDL_WINDOWEVENT_MOVED:
                        {
                        NewEvent.EventType = WindowEventType::WindowMoved;
                        NewEvent.EventData.WindowMoved.X = Event.window.data1;
                        NewEvent.EventData.WindowMoved.Y = Event.window.data2;

                        Windows[NewEvent.OwnerHandle].Position = glm::ivec2 ( Event.window.data1, Event.window.data2 );
                        break;
                        }

                    case SDL_WINDOWEVENT_RESIZED:
                        {
                        NewEvent.EventType = WindowEventType::WindowResized;
                        NewEvent.EventData.WindowResized.Width = Event.window.data1;
                        NewEvent.EventData.WindowResized.Height = Event.window.data2;

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
}
}
}
