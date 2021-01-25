#include "WindowsBackend.h"
#include "../../Logger.h"
#include <unordered_map>
#include <map>
#include <algorithm>

namespace CrossRenderer
{
namespace WindowManager
{
namespace Windows
{
typedef struct WindowsWindowInfo
    {
    HWND Window;
    HDC DC;
    WNDCLASSEX WindowClass;
    glm::ivec2 Position;
    glm::uvec2 Size;
    std::string Title;
    WindowState State;
    } WindowsWindowInfo;

static VectorizedContainer <WindowsWindowInfo, RenderWindowTag> Windows;
static std::vector <MSG> PendingWindowsMessages;
static std::unordered_map <HWND, RenderWindowHandle> HWNDToHandleMap;
static std::map <uint32_t, std::string> KeyNameToCodeMap;
static std::map <std::string, uint32_t> KeyCodeToNameMap;
static uint8_t MouseButtonStatus = 0;
static glm::ivec2 MouseCursorPosition;

const char *StringifyWindowsMessage ( const UINT Value );
void InitializeKeyNames ( void );
void ProcessSingleMessage ( const MSG &Message );

LRESULT CALLBACK WindowProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
    MSG msg;
    msg.hwnd = hwnd;
    msg.lParam = lParam;
    msg.wParam = wParam;
    msg.message = uMsg;
    PendingWindowsMessages.push_back ( std::move ( msg ) );
    return DefWindowProc ( msg.hwnd, msg.message, msg.wParam, msg.lParam );
    }

RenderWindowHandle CreateNewWindow ( const RenderWindowDescriptor &Descriptor )
    {
    WNDCLASSEX WindowClass;
    if ( !GetClassInfoEx ( GetModuleHandle ( nullptr ), "CrossRendererWindowClass", &WindowClass ) )
        {
        memset ( &WindowClass, 0, sizeof ( WNDCLASSEX ) );
        WindowClass.cbSize = sizeof ( WNDCLASSEX );
        WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        WindowClass.lpfnWndProc = WindowProc;
        WindowClass.cbClsExtra = 0;
        WindowClass.cbWndExtra = 0;
        WindowClass.hInstance = GetModuleHandle ( nullptr );
        WindowClass.hIcon = LoadIcon ( nullptr, IDI_APPLICATION );
        WindowClass.hCursor = LoadCursor ( nullptr, IDC_ARROW );
        WindowClass.hbrBackground = ( HBRUSH ) GetStockObject ( BLACK_BRUSH );
        WindowClass.lpszMenuName = nullptr;
        WindowClass.lpszClassName = "CrossRendererWindowClass";
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

    WindowsWindowInfo NewWindow;
    DWORD dwExStyle;
    DWORD dwStyle;
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    RECT windowRect;
    windowRect.left = Descriptor.Position.x;
    windowRect.top = Descriptor.Position.y;
    windowRect.right = Descriptor.Size.x;
    windowRect.bottom = Descriptor.Size.y;

    AdjustWindowRectEx ( &windowRect, dwStyle, FALSE, dwExStyle );

    NewWindow.Window = CreateWindowEx ( 0, "CrossRendererWindowClass", Descriptor.Title.c_str (), dwStyle,
                                        0, 0, windowRect.right - windowRect.left,
                                        windowRect.bottom - windowRect.top, nullptr, nullptr,
                                        nullptr, nullptr );

    if ( !NewWindow.Window )
        {
        printf ( "Could not create window! %lX\n", GetLastError () );
        fflush ( stdout );
        exit ( -1 );
        }

    ShowWindow ( NewWindow.Window, SW_SHOWNORMAL );
    SetForegroundWindow ( NewWindow.Window );
    SetFocus ( NewWindow.Window );

    NewWindow.Size = Descriptor.Size;
    NewWindow.Position = Descriptor.Position;
    NewWindow.Title = Descriptor.Title;
    NewWindow.DC = GetDC ( NewWindow.Window );
    NewWindow.State = WindowState::Normal;
    RenderWindowHandle NewHandle = Windows.GetNewHandle ();
    Windows[NewHandle] = NewWindow;
    HWNDToHandleMap.insert ( std::pair <HWND, RenderWindowHandle> ( NewWindow.Window, NewHandle ) );
    WindowManager::WindowList.insert ( NewHandle );
    return NewHandle;
    }

bool DestroyWindow ( const RenderWindowHandle &Handle )
    {
    WindowsWindowInfo *info = &Windows[Handle];

    HWNDToHandleMap.erase ( HWNDToHandleMap.find ( info->Window ) );
    Windows.ReleaseHandle ( Handle );
    WindowList.erase ( Handle );

    ::DestroyWindow ( info->Window );
    return true;
    }

void SetWindowPosition ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    SetWindowPos ( info->Window, nullptr, NewPosition.x, NewPosition.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
    info->Position = NewPosition;
    }

glm::ivec2 GetWindowPosition ( const RenderWindowHandle &Handle )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    return info->Position;

    //RECT Rect;
    //GetWindowRect ( info->Window, &Rect );
    //return glm::ivec2 ( Rect.left, Rect.top );
    }

void SetWindowSize ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    SetWindowPos ( info->Window, nullptr, 0, 0, NewSize.x, NewSize.y, SWP_NOZORDER | SWP_NOMOVE );
    info->Size = NewSize;
    }

glm::uvec2 GetWindowSize ( const RenderWindowHandle &Handle )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    return info->Size;

    //RECT Rect;
    //GetWindowRect ( info->Window, &Rect );
    //return glm::uvec2 ( Rect.right - Rect.left, Rect.bottom - Rect.top );
    }

void SetWindowTitle ( const RenderWindowHandle &Handle, const std::string &NewTitle )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    SetWindowText ( info->Window, NewTitle.c_str () );
    info->Title = NewTitle;
    }

std::string GetWindowTitle ( const RenderWindowHandle &Handle )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    return info->Title;
    //std::string Title;
    //size_t Length = GetWindowTextLength ( info->Window );
    //Title.resize ( Length );
    //GetWindowText ( info->Window, const_cast <char*> ( Title.c_str() ), Length );
    //return Title;
    }

bool SetWindowState ( const RenderWindowHandle &Handle, const WindowState NewState )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    switch ( NewState )
        {
        case WindowState::Minimized:
            ShowWindow ( info->Window, SW_SHOWMINIMIZED );
            break;
        case WindowState::Normal:
            ShowWindow ( info->Window, SW_SHOWNORMAL );
            break;
        case WindowState::Maximized:
            ShowWindow ( info->Window, SW_SHOWMAXIMIZED );
            break;
        }
    info->State = NewState;
    return true;
    }

WindowState GetWindowState ( const RenderWindowHandle &Handle )
    {
    WindowsWindowInfo *info = &Windows[Handle];
    WINDOWPLACEMENT Placement;
    memset ( &Placement, 0, sizeof ( Placement ) );
    Placement.length = sizeof ( WINDOWPLACEMENT );
    GetWindowPlacement ( info->Window, &Placement );
    if ( ( Placement.showCmd & SW_MAXIMIZE ) == SW_MAXIMIZE )
        return WindowState::Maximized;
    if ( ( Placement.showCmd & SW_SHOWNORMAL ) == SW_SHOWNORMAL )
        return WindowState::Normal;
    if ( ( Placement.showCmd & SW_SHOWMINIMIZED ) == SW_SHOWMINIMIZED )
        return WindowState::Minimized;
    return WindowState::Normal;
    }

void ProcessEvents ( void )
    {
    MSG msg = {};

    while ( PeekMessage ( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
        switch ( msg.message )
            {
            case WM_QUIT:
                {
                WindowEvent NewEvent;
                NewEvent.EventType = WindowEventType::Quit;
                SendWindowEvent ( NewEvent );
                return;
                }
            default:
                TranslateMessage ( &msg );
                DispatchMessage ( &msg );
                break;
            }
        }
    for ( auto &CurrentMessage : PendingWindowsMessages )
        {
        ProcessSingleMessage ( CurrentMessage );
        }
    PendingWindowsMessages.clear ();
    }

uint32_t GetKeyCode ( const std::string KeyName )
    {
    if ( KeyName.length () == 1 )
        {
        if ( ( KeyName[0] >= 'A' ) && ( KeyName[0] <= 'Z' ) )
            {
            return KeyName[0] + 32; // Convert to lowercase
            }
        return KeyName[0];
        }

    if ( KeyCodeToNameMap.empty () )
        {
        InitializeKeyNames ();
        }
    std::string LowerCaseName = KeyName;
    std::transform ( KeyName.begin (), KeyName.end (), LowerCaseName.begin (), [] ( unsigned char c ) -> unsigned char { return ( unsigned char ) std::tolower ( c ); } );
    auto FindResult = KeyCodeToNameMap.find ( LowerCaseName );
    if ( FindResult != KeyCodeToNameMap.end () )
        return FindResult->second;

    return 0;
    }

uint32_t GetKeyScanCode ( const std::string )
    {
    return 0;
    }
const char *GetKeyName ( const uint32_t KeyCode )
    {
    if ( isprint ( KeyCode ) )
        {
        static char KeyName[2] = " ";
        KeyName[0] = ( char ) KeyCode;
        if ( ( KeyName[0] >= 'A' ) && ( KeyName[0] <= 'Z' ) )
            KeyName[0] += 32; // Convert to lowercase
        return KeyName;
        }

    if ( KeyNameToCodeMap.empty () )
        {
        InitializeKeyNames ();
        }

    auto FindResult = KeyNameToCodeMap.find ( KeyCode );
    if ( FindResult != KeyNameToCodeMap.end () )
        return FindResult->second.c_str();

    return nullptr;
    }

glm::ivec2 GetMousePosition ( void )
    {
    return MouseCursorPosition;
    }

uint32_t GetMouseButtonStatus ( void )
    {
    return MouseButtonStatus;
    }

std::string GetErrorDescription ( void )
    {
    char *lpMsgBuf;
    DWORD dw = GetLastError ();

    FormatMessage (
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dw,
        MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        ( LPTSTR ) &lpMsgBuf,
        0, nullptr );
    std::string Description;
    Description.assign ( lpMsgBuf );
    LocalFree ( lpMsgBuf );
    return Description;
    }

HWND GetWindowHandle ( const RenderWindowHandle &Handle )
    {
    return Windows[Handle].Window;
    }

void ProcessSingleMessage ( const MSG &Message )
    {
    static std::pair <HWND, RenderWindowHandle> CachedHandle ( 0, RenderWindowHandle () );
    if ( Message.hwnd != CachedHandle.first )
        {
        std::unordered_map<HWND, RenderWindowHandle>::iterator HandleIterator = HWNDToHandleMap.find ( Message.hwnd );
        if ( HandleIterator == HWNDToHandleMap.end () )
            {
            LOG_ERROR ( "Message '%s' for unknown window handle '%X'", StringifyWindowsMessage ( Message.message ), Message.hwnd );
            return;
            }
        CachedHandle.first = HandleIterator->first;
        CachedHandle.second = HandleIterator->second;
        }

    WindowEvent NewEvent;
    NewEvent.OwnerHandle = CachedHandle.second;
    switch ( Message.message )
        {
        default:
            return;

        case WM_CREATE:
            {
            NewEvent.EventType = WindowEventType::WindowCreated;
            break;
            }
        case WM_CLOSE:
            {
            NewEvent.EventType = WindowEventType::WindowClosed;
            HWNDToHandleMap.erase ( HWNDToHandleMap.find ( Message.hwnd ) );
            Windows.ReleaseHandle ( NewEvent.OwnerHandle );
            WindowList.erase ( NewEvent.OwnerHandle );
            break;
            }
        case WM_MOVE:
            {
            NewEvent.EventType = WindowEventType::WindowMoved;
            NewEvent.EventData.WindowMoved.X = LOWORD ( Message.lParam );
            NewEvent.EventData.WindowMoved.Y = HIWORD ( Message.lParam );
            Windows[NewEvent.OwnerHandle].Position = glm::ivec2 ( NewEvent.EventData.WindowMoved.X, NewEvent.EventData.WindowMoved.Y );
            break;
            }
        case WM_SIZE:
            {
            NewEvent.EventType = WindowEventType::WindowResized;
            NewEvent.EventData.WindowResized.Width = LOWORD ( Message.lParam );
            NewEvent.EventData.WindowResized.Height = HIWORD ( Message.lParam );
            Windows[NewEvent.OwnerHandle].Size = glm::uvec2 ( NewEvent.EventData.WindowResized.Width, NewEvent.EventData.WindowResized.Height );
            break;
            }
        case WM_MOUSEMOVE:
            {
            NewEvent.EventType = WindowEventType::MouseMoved;
            NewEvent.EventData.MouseMoved.X = ( int16_t ) LOWORD ( Message.lParam );
            NewEvent.EventData.MouseMoved.Y = ( int16_t ) HIWORD ( Message.lParam );
            MouseCursorPosition.x = (int16_t)LOWORD ( Message.lParam );
            MouseCursorPosition.y = (int16_t)HIWORD ( Message.lParam );
            break;
            }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            {
            NewEvent.EventType = ( Message.message == WM_LBUTTONDOWN ) ? WindowEventType::ButtonPressed : WindowEventType::ButtonReleased;
            NewEvent.EventData.ButtonPressed.Button = 0;
            if ( Message.message == WM_LBUTTONDOWN )
                MouseButtonStatus |= 1;
            else
                MouseButtonStatus ^= 1;
            break;
            }
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            {
            NewEvent.EventType = ( Message.message == WM_RBUTTONDOWN ) ? WindowEventType::ButtonPressed : WindowEventType::ButtonReleased;
            NewEvent.EventData.ButtonPressed.Button = 1;
            if ( Message.message == WM_RBUTTONDOWN )
                MouseButtonStatus |= 2;
            else
                MouseButtonStatus ^= 2;
            break;
            }
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            {
            NewEvent.EventType = ( Message.message == WM_MBUTTONDOWN ) ? WindowEventType::ButtonPressed : WindowEventType::ButtonReleased;
            NewEvent.EventData.ButtonPressed.Button = 2;
            if ( Message.message == WM_MBUTTONDOWN )
                MouseButtonStatus |= 4;
            else
                MouseButtonStatus ^= 4;
            break;
            }
        case WM_MOUSEWHEEL:
            {
            NewEvent.EventType = WindowEventType::MouseWheel;
            NewEvent.EventData.MouseWheel.X = ( int16_t ) HIWORD ( Message.wParam );
            NewEvent.EventData.MouseWheel.X /= WHEEL_DELTA;
            NewEvent.EventData.MouseWheel.Y = 0;
            break;
            }
        case WM_KEYDOWN:
        case WM_KEYUP:
            {
            TranslateMessage ( &Message );
            DispatchMessage ( &Message );
            if ( ( Message.message == WM_KEYDOWN ) && ( Message.lParam >> 30 & 1 ) ) // if the key was previously down already, just ignore this. It's a repeated message
                return;
            NewEvent.EventType = ( Message.message == WM_KEYDOWN ) ? WindowEventType::KeyPressed : WindowEventType::KeyReleased;
            NewEvent.EventData.KeyPressed.Key = ( uint32_t ) Message.wParam;
            if ( ( Message.wParam >= 'A' ) && ( Message.wParam <= 'Z' ) )
                NewEvent.EventData.KeyPressed.Key += 32; // Convert to lowercase
            break;
            }
        }
    SendWindowEvent ( NewEvent );
    }

const char *StringifyWindowsMessage ( const UINT Value )
    {
#define STRINGIFY(X) case X: return #X;
    switch ( Value )
        {
            STRINGIFY ( WM_ACTIVATE );
            STRINGIFY ( WM_ACTIVATEAPP );
            STRINGIFY ( WM_AFXFIRST );
            STRINGIFY ( WM_AFXLAST );
            STRINGIFY ( WM_APP );
            STRINGIFY ( WM_APPCOMMAND );
            STRINGIFY ( WM_ASKCBFORMATNAME );
            STRINGIFY ( WM_CANCELJOURNAL );
            STRINGIFY ( WM_CANCELMODE );
            STRINGIFY ( WM_CAPTURECHANGED );
            STRINGIFY ( WM_CHANGECBCHAIN );
            STRINGIFY ( WM_CHANGEUISTATE );
            STRINGIFY ( WM_CHAR );
            STRINGIFY ( WM_CHARTOITEM );
            STRINGIFY ( WM_CHILDACTIVATE );
            STRINGIFY ( WM_CHOOSEFONT_GETLOGFONT );
            STRINGIFY ( WM_CHOOSEFONT_SETFLAGS );
            STRINGIFY ( WM_CHOOSEFONT_SETLOGFONT );
            STRINGIFY ( WM_CLEAR );
            STRINGIFY ( WM_CLOSE );
            STRINGIFY ( WM_COMMAND );
            STRINGIFY ( WM_COMMNOTIFY );
            STRINGIFY ( WM_COMPACTING );
            STRINGIFY ( WM_COMPAREITEM );
            STRINGIFY ( WM_CONTEXTMENU );
            STRINGIFY ( WM_COPY );
            STRINGIFY ( WM_COPYDATA );
            STRINGIFY ( WM_CREATE );
            STRINGIFY ( WM_CTLCOLORBTN );
            STRINGIFY ( WM_CTLCOLORDLG );
            STRINGIFY ( WM_CTLCOLOREDIT );
            STRINGIFY ( WM_CTLCOLORLISTBOX );
            STRINGIFY ( WM_CTLCOLORMSGBOX );
            STRINGIFY ( WM_CTLCOLORSCROLLBAR );
            STRINGIFY ( WM_CTLCOLORSTATIC );
            STRINGIFY ( WM_CUT );
            STRINGIFY ( WM_DEADCHAR );
            STRINGIFY ( WM_DELETEITEM );
            STRINGIFY ( WM_DESTROY );
            STRINGIFY ( WM_DESTROYCLIPBOARD );
            STRINGIFY ( WM_DEVICECHANGE );
            STRINGIFY ( WM_DEVMODECHANGE );
            STRINGIFY ( WM_DISPLAYCHANGE );
            STRINGIFY ( WM_DRAWCLIPBOARD );
            STRINGIFY ( WM_DRAWITEM );
            STRINGIFY ( WM_DROPFILES );
            STRINGIFY ( WM_ENABLE );
            STRINGIFY ( WM_ENDSESSION );
            STRINGIFY ( WM_ENTERIDLE );
            STRINGIFY ( WM_ENTERMENULOOP );
            STRINGIFY ( WM_ENTERSIZEMOVE );
            STRINGIFY ( WM_ERASEBKGND );
            STRINGIFY ( WM_EXITMENULOOP );
            STRINGIFY ( WM_EXITSIZEMOVE );
            STRINGIFY ( WM_FONTCHANGE );
            STRINGIFY ( WM_GETDLGCODE );
            STRINGIFY ( WM_GETFONT );
            STRINGIFY ( WM_GETHOTKEY );
            STRINGIFY ( WM_GETICON );
            STRINGIFY ( WM_GETMINMAXINFO );
            STRINGIFY ( WM_GETOBJECT );
            STRINGIFY ( WM_GETTEXT );
            STRINGIFY ( WM_GETTEXTLENGTH );
            STRINGIFY ( WM_HANDHELDFIRST );
            STRINGIFY ( WM_HANDHELDLAST );
            STRINGIFY ( WM_HELP );
            STRINGIFY ( WM_HOTKEY );
            STRINGIFY ( WM_HSCROLL );
            STRINGIFY ( WM_HSCROLLCLIPBOARD );
            STRINGIFY ( WM_ICONERASEBKGND );
            STRINGIFY ( WM_IME_CHAR );
            STRINGIFY ( WM_IME_COMPOSITION );
            STRINGIFY ( WM_IME_COMPOSITIONFULL );
            STRINGIFY ( WM_IME_CONTROL );
            STRINGIFY ( WM_IME_ENDCOMPOSITION );
            STRINGIFY ( WM_IME_KEYDOWN );
            STRINGIFY ( WM_IME_KEYUP );
            STRINGIFY ( WM_IME_NOTIFY );
            STRINGIFY ( WM_IME_REQUEST );
            STRINGIFY ( WM_IME_SELECT );
            STRINGIFY ( WM_IME_SETCONTEXT );
            STRINGIFY ( WM_IME_STARTCOMPOSITION );
            STRINGIFY ( WM_INITDIALOG );
            STRINGIFY ( WM_INITMENU );
            STRINGIFY ( WM_INITMENUPOPUP );
            STRINGIFY ( WM_INPUT );
            STRINGIFY ( WM_INPUTLANGCHANGE );
            STRINGIFY ( WM_INPUTLANGCHANGEREQUEST );
            STRINGIFY ( WM_KEYDOWN );
            STRINGIFY ( WM_KEYLAST );
            STRINGIFY ( WM_KEYUP );
            STRINGIFY ( WM_KILLFOCUS );
            STRINGIFY ( WM_LBUTTONDBLCLK );
            STRINGIFY ( WM_LBUTTONDOWN );
            STRINGIFY ( WM_LBUTTONUP );
            STRINGIFY ( WM_MBUTTONDBLCLK );
            STRINGIFY ( WM_MBUTTONDOWN );
            STRINGIFY ( WM_MBUTTONUP );
            STRINGIFY ( WM_MDIACTIVATE );
            STRINGIFY ( WM_MDICASCADE );
            STRINGIFY ( WM_MDICREATE );
            STRINGIFY ( WM_MDIDESTROY );
            STRINGIFY ( WM_MDIGETACTIVE );
            STRINGIFY ( WM_MDIICONARRANGE );
            STRINGIFY ( WM_MDIMAXIMIZE );
            STRINGIFY ( WM_MDINEXT );
            STRINGIFY ( WM_MDIREFRESHMENU );
            STRINGIFY ( WM_MDIRESTORE );
            STRINGIFY ( WM_MDISETMENU );
            STRINGIFY ( WM_MDITILE );
            STRINGIFY ( WM_MEASUREITEM );
            STRINGIFY ( WM_MENUCHAR );
            STRINGIFY ( WM_MENUCOMMAND );
            STRINGIFY ( WM_MENUDRAG );
            STRINGIFY ( WM_MENUGETOBJECT );
            STRINGIFY ( WM_MENURBUTTONUP );
            STRINGIFY ( WM_MENUSELECT );
            STRINGIFY ( WM_MOUSEACTIVATE );
            STRINGIFY ( WM_MOUSEHOVER );
            STRINGIFY ( WM_MOUSELEAVE );
            STRINGIFY ( WM_MOUSEMOVE );
            STRINGIFY ( WM_MOUSEWHEEL );
            STRINGIFY ( WM_MOVE );
            STRINGIFY ( WM_MOVING );
            STRINGIFY ( WM_NCACTIVATE );
            STRINGIFY ( WM_NCCALCSIZE );
            STRINGIFY ( WM_NCCREATE );
            STRINGIFY ( WM_NCDESTROY );
            STRINGIFY ( WM_NCHITTEST );
            STRINGIFY ( WM_NCLBUTTONDBLCLK );
            STRINGIFY ( WM_NCLBUTTONDOWN );
            STRINGIFY ( WM_NCLBUTTONUP );
            STRINGIFY ( WM_NCMBUTTONDBLCLK );
            STRINGIFY ( WM_NCMBUTTONDOWN );
            STRINGIFY ( WM_NCMBUTTONUP );
            STRINGIFY ( WM_NCMOUSEHOVER );
            STRINGIFY ( WM_NCMOUSELEAVE );
            STRINGIFY ( WM_NCMOUSEMOVE );
            STRINGIFY ( WM_NCPAINT );
            STRINGIFY ( WM_NCRBUTTONDBLCLK );
            STRINGIFY ( WM_NCRBUTTONDOWN );
            STRINGIFY ( WM_NCRBUTTONUP );
            STRINGIFY ( WM_NCXBUTTONDBLCLK );
            STRINGIFY ( WM_NCXBUTTONDOWN );
            STRINGIFY ( WM_NCXBUTTONUP );
            STRINGIFY ( WM_NEXTDLGCTL );
            STRINGIFY ( WM_NEXTMENU );
            STRINGIFY ( WM_NOTIFY );
            STRINGIFY ( WM_NOTIFYFORMAT );
            STRINGIFY ( WM_NULL );
            STRINGIFY ( WM_PAINT );
            STRINGIFY ( WM_PAINTCLIPBOARD );
            STRINGIFY ( WM_PAINTICON );
            STRINGIFY ( WM_PALETTECHANGED );
            STRINGIFY ( WM_PALETTEISCHANGING );
            STRINGIFY ( WM_PARENTNOTIFY );
            STRINGIFY ( WM_PASTE );
            STRINGIFY ( WM_PENWINFIRST );
            STRINGIFY ( WM_PENWINLAST );
            STRINGIFY ( WM_POWER );
            STRINGIFY ( WM_POWERBROADCAST );
            STRINGIFY ( WM_PRINT );
            STRINGIFY ( WM_PRINTCLIENT );
            STRINGIFY ( WM_PSD_ENVSTAMPRECT );
            STRINGIFY ( WM_PSD_GREEKTEXTRECT );
            STRINGIFY ( WM_PSD_MARGINRECT );
            STRINGIFY ( WM_PSD_MINMARGINRECT );
            STRINGIFY ( WM_PSD_PAGESETUPDLG );
            STRINGIFY ( WM_PSD_YAFULLPAGERECT );
            STRINGIFY ( WM_QUERYDRAGICON );
            STRINGIFY ( WM_QUERYENDSESSION );
            STRINGIFY ( WM_QUERYNEWPALETTE );
            STRINGIFY ( WM_QUERYOPEN );
            STRINGIFY ( WM_QUERYUISTATE );
            STRINGIFY ( WM_QUEUESYNC );
            STRINGIFY ( WM_QUIT );
            STRINGIFY ( WM_RBUTTONDBLCLK );
            STRINGIFY ( WM_RBUTTONDOWN );
            STRINGIFY ( WM_RBUTTONUP );
            STRINGIFY ( WM_RENDERALLFORMATS );
            STRINGIFY ( WM_RENDERFORMAT );
            STRINGIFY ( WM_SETCURSOR );
            STRINGIFY ( WM_SETFOCUS );
            STRINGIFY ( WM_SETFONT );
            STRINGIFY ( WM_SETHOTKEY );
            STRINGIFY ( WM_SETICON );
            STRINGIFY ( WM_SETREDRAW );
            STRINGIFY ( WM_SETTEXT );
            STRINGIFY ( WM_SHOWWINDOW );
            STRINGIFY ( WM_SIZE );
            STRINGIFY ( WM_SIZECLIPBOARD );
            STRINGIFY ( WM_SIZING );
            STRINGIFY ( WM_SPOOLERSTATUS );
            STRINGIFY ( WM_STYLECHANGED );
            STRINGIFY ( WM_STYLECHANGING );
            STRINGIFY ( WM_SYNCPAINT );
            STRINGIFY ( WM_SYSCHAR );
            STRINGIFY ( WM_SYSCOLORCHANGE );
            STRINGIFY ( WM_SYSCOMMAND );
            STRINGIFY ( WM_SYSDEADCHAR );
            STRINGIFY ( WM_SYSKEYDOWN );
            STRINGIFY ( WM_SYSKEYUP );
            STRINGIFY ( WM_TCARD );
            STRINGIFY ( WM_TIMECHANGE );
            STRINGIFY ( WM_TIMER );
            STRINGIFY ( WM_UNDO );
            STRINGIFY ( WM_UNINITMENUPOPUP );
            STRINGIFY ( WM_UPDATEUISTATE );
            STRINGIFY ( WM_USERCHANGED );
            STRINGIFY ( WM_VKEYTOITEM );
            STRINGIFY ( WM_VSCROLL );
            STRINGIFY ( WM_VSCROLLCLIPBOARD );
            STRINGIFY ( WM_WINDOWPOSCHANGED );
            STRINGIFY ( WM_WINDOWPOSCHANGING );
            STRINGIFY ( WM_WININICHANGE );
            STRINGIFY ( WM_XBUTTONDBLCLK );
            STRINGIFY ( WM_XBUTTONDOWN );
            STRINGIFY ( WM_XBUTTONUP );
        }
#undef STRINGIFY
    return "";
    }

void InitializeKeyNames ( void )
    {
    KeyNameToCodeMap.clear ();
    KeyCodeToNameMap.clear ();
#define ADD_NAME(KEYCODE)\
{\
    std::string Name = std::string(#KEYCODE).substr(3);\
    std::transform(Name.begin(), Name.end(), Name.begin(), [] ( unsigned char c ) -> unsigned char { return (unsigned char) std::tolower ( c ); } );\
    KeyNameToCodeMap.insert ( std::pair <uint32_t,std::string> ( KEYCODE, Name ));\
KeyCodeToNameMap.insert ( std::pair <std::string, uint32_t> ( Name,KEYCODE ));\
}

    ADD_NAME ( VK_LBUTTON );
    ADD_NAME ( VK_RBUTTON );
    ADD_NAME ( VK_CANCEL );
    ADD_NAME ( VK_MBUTTON );
    ADD_NAME ( VK_XBUTTON1 );
    ADD_NAME ( VK_XBUTTON2 );
    ADD_NAME ( VK_BACK );
    ADD_NAME ( VK_TAB );
    ADD_NAME ( VK_CLEAR );
    ADD_NAME ( VK_RETURN );
    ADD_NAME ( VK_SHIFT );
    ADD_NAME ( VK_CONTROL );
    ADD_NAME ( VK_MENU );
    ADD_NAME ( VK_PAUSE );
    ADD_NAME ( VK_CAPITAL );
    ADD_NAME ( VK_KANA );
    ADD_NAME ( VK_HANGEUL );
    ADD_NAME ( VK_HANGUL );
    ADD_NAME ( VK_JUNJA );
    ADD_NAME ( VK_FINAL );
    ADD_NAME ( VK_HANJA );
    ADD_NAME ( VK_KANJI );
    ADD_NAME ( VK_ESCAPE );
    ADD_NAME ( VK_CONVERT );
    ADD_NAME ( VK_NONCONVERT );
    ADD_NAME ( VK_ACCEPT );
    ADD_NAME ( VK_MODECHANGE );
    ADD_NAME ( VK_SPACE );
    ADD_NAME ( VK_PRIOR );
    ADD_NAME ( VK_NEXT );
    ADD_NAME ( VK_END );
    ADD_NAME ( VK_HOME );
    ADD_NAME ( VK_LEFT );
    ADD_NAME ( VK_UP );
    ADD_NAME ( VK_RIGHT );
    ADD_NAME ( VK_DOWN );
    ADD_NAME ( VK_SELECT );
    ADD_NAME ( VK_PRINT );
    ADD_NAME ( VK_EXECUTE );
    ADD_NAME ( VK_SNAPSHOT );
    ADD_NAME ( VK_INSERT );
    ADD_NAME ( VK_DELETE );
    ADD_NAME ( VK_HELP );
    ADD_NAME ( VK_LWIN );
    ADD_NAME ( VK_RWIN );
    ADD_NAME ( VK_APPS );
    ADD_NAME ( VK_SLEEP );
    ADD_NAME ( VK_NUMPAD0 );
    ADD_NAME ( VK_NUMPAD1 );
    ADD_NAME ( VK_NUMPAD2 );
    ADD_NAME ( VK_NUMPAD3 );
    ADD_NAME ( VK_NUMPAD4 );
    ADD_NAME ( VK_NUMPAD5 );
    ADD_NAME ( VK_NUMPAD6 );
    ADD_NAME ( VK_NUMPAD7 );
    ADD_NAME ( VK_NUMPAD8 );
    ADD_NAME ( VK_NUMPAD9 );
    ADD_NAME ( VK_MULTIPLY );
    ADD_NAME ( VK_ADD );
    ADD_NAME ( VK_SEPARATOR );
    ADD_NAME ( VK_SUBTRACT );
    ADD_NAME ( VK_DECIMAL );
    ADD_NAME ( VK_DIVIDE );
    ADD_NAME ( VK_F1 );
    ADD_NAME ( VK_F2 );
    ADD_NAME ( VK_F3 );
    ADD_NAME ( VK_F4 );
    ADD_NAME ( VK_F5 );
    ADD_NAME ( VK_F6 );
    ADD_NAME ( VK_F7 );
    ADD_NAME ( VK_F8 );
    ADD_NAME ( VK_F9 );
    ADD_NAME ( VK_F10 );
    ADD_NAME ( VK_F11 );
    ADD_NAME ( VK_F12 );
    ADD_NAME ( VK_F13 );
    ADD_NAME ( VK_F14 );
    ADD_NAME ( VK_F15 );
    ADD_NAME ( VK_F16 );
    ADD_NAME ( VK_F17 );
    ADD_NAME ( VK_F18 );
    ADD_NAME ( VK_F19 );
    ADD_NAME ( VK_F20 );
    ADD_NAME ( VK_F21 );
    ADD_NAME ( VK_F22 );
    ADD_NAME ( VK_F23 );
    ADD_NAME ( VK_F24 );
    ADD_NAME ( VK_NAVIGATION_VIEW );
    ADD_NAME ( VK_NAVIGATION_MENU );
    ADD_NAME ( VK_NAVIGATION_UP );
    ADD_NAME ( VK_NAVIGATION_DOWN );
    ADD_NAME ( VK_NAVIGATION_LEFT );
    ADD_NAME ( VK_NAVIGATION_RIGHT );
    ADD_NAME ( VK_NAVIGATION_ACCEPT );
    ADD_NAME ( VK_NAVIGATION_CANCEL );
    ADD_NAME ( VK_NUMLOCK );
    ADD_NAME ( VK_SCROLL );
    ADD_NAME ( VK_OEM_NEC_EQUAL );
    ADD_NAME ( VK_OEM_FJ_JISHO );
    ADD_NAME ( VK_OEM_FJ_MASSHOU );
    ADD_NAME ( VK_OEM_FJ_TOUROKU );
    ADD_NAME ( VK_OEM_FJ_LOYA );
    ADD_NAME ( VK_OEM_FJ_ROYA );
    ADD_NAME ( VK_LSHIFT );
    ADD_NAME ( VK_RSHIFT );
    ADD_NAME ( VK_LCONTROL );
    ADD_NAME ( VK_RCONTROL );
    ADD_NAME ( VK_LMENU );
    ADD_NAME ( VK_RMENU );
    ADD_NAME ( VK_BROWSER_BACK );
    ADD_NAME ( VK_BROWSER_FORWARD );
    ADD_NAME ( VK_BROWSER_REFRESH );
    ADD_NAME ( VK_BROWSER_STOP );
    ADD_NAME ( VK_BROWSER_SEARCH );
    ADD_NAME ( VK_BROWSER_FAVORITES );
    ADD_NAME ( VK_BROWSER_HOME );
    ADD_NAME ( VK_VOLUME_MUTE );
    ADD_NAME ( VK_VOLUME_DOWN );
    ADD_NAME ( VK_VOLUME_UP );
    ADD_NAME ( VK_MEDIA_NEXT_TRACK );
    ADD_NAME ( VK_MEDIA_PREV_TRACK );
    ADD_NAME ( VK_MEDIA_STOP );
    ADD_NAME ( VK_MEDIA_PLAY_PAUSE );
    ADD_NAME ( VK_LAUNCH_MAIL );
    ADD_NAME ( VK_LAUNCH_MEDIA_SELECT );
    ADD_NAME ( VK_LAUNCH_APP1 );
    ADD_NAME ( VK_LAUNCH_APP2 );
    ADD_NAME ( VK_OEM_1 );
    ADD_NAME ( VK_OEM_PLUS );
    ADD_NAME ( VK_OEM_COMMA );
    ADD_NAME ( VK_OEM_MINUS );
    ADD_NAME ( VK_OEM_PERIOD );
    ADD_NAME ( VK_OEM_2 );
    ADD_NAME ( VK_OEM_3 );
    ADD_NAME ( VK_GAMEPAD_A );
    ADD_NAME ( VK_GAMEPAD_B );
    ADD_NAME ( VK_GAMEPAD_X );
    ADD_NAME ( VK_GAMEPAD_Y );
    ADD_NAME ( VK_GAMEPAD_RIGHT_SHOULDER );
    ADD_NAME ( VK_GAMEPAD_LEFT_SHOULDER );
    ADD_NAME ( VK_GAMEPAD_LEFT_TRIGGER );
    ADD_NAME ( VK_GAMEPAD_RIGHT_TRIGGER );
    ADD_NAME ( VK_GAMEPAD_DPAD_UP );
    ADD_NAME ( VK_GAMEPAD_DPAD_DOWN );
    ADD_NAME ( VK_GAMEPAD_DPAD_LEFT );
    ADD_NAME ( VK_GAMEPAD_DPAD_RIGHT );
    ADD_NAME ( VK_GAMEPAD_MENU );
    ADD_NAME ( VK_GAMEPAD_VIEW );
    ADD_NAME ( VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON );
    ADD_NAME ( VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON );
    ADD_NAME ( VK_GAMEPAD_LEFT_THUMBSTICK_UP );
    ADD_NAME ( VK_GAMEPAD_LEFT_THUMBSTICK_DOWN );
    ADD_NAME ( VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT );
    ADD_NAME ( VK_GAMEPAD_LEFT_THUMBSTICK_LEFT );
    ADD_NAME ( VK_GAMEPAD_RIGHT_THUMBSTICK_UP );
    ADD_NAME ( VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN );
    ADD_NAME ( VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT );
    ADD_NAME ( VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT );
    ADD_NAME ( VK_OEM_4 );
    ADD_NAME ( VK_OEM_5 );
    ADD_NAME ( VK_OEM_6 );
    ADD_NAME ( VK_OEM_7 );
    ADD_NAME ( VK_OEM_8 );
    ADD_NAME ( VK_OEM_AX );
    ADD_NAME ( VK_OEM_102 );
    ADD_NAME ( VK_ICO_HELP );
    ADD_NAME ( VK_ICO_00 );
    ADD_NAME ( VK_PROCESSKEY );
    ADD_NAME ( VK_ICO_CLEAR );
    ADD_NAME ( VK_PACKET );
    ADD_NAME ( VK_OEM_RESET );
    ADD_NAME ( VK_OEM_JUMP );
    ADD_NAME ( VK_OEM_PA1 );
    ADD_NAME ( VK_OEM_PA2 );
    ADD_NAME ( VK_OEM_PA3 );
    ADD_NAME ( VK_OEM_WSCTRL );
    ADD_NAME ( VK_OEM_CUSEL );
    ADD_NAME ( VK_OEM_ATTN );
    ADD_NAME ( VK_OEM_FINISH );
    ADD_NAME ( VK_OEM_COPY );
    ADD_NAME ( VK_OEM_AUTO );
    ADD_NAME ( VK_OEM_ENLW );
    ADD_NAME ( VK_OEM_BACKTAB );
    ADD_NAME ( VK_ATTN );
    ADD_NAME ( VK_CRSEL );
    ADD_NAME ( VK_EXSEL );
    ADD_NAME ( VK_EREOF );
    ADD_NAME ( VK_PLAY );
    ADD_NAME ( VK_ZOOM );
    ADD_NAME ( VK_NONAME );
    ADD_NAME ( VK_PA1 );
    ADD_NAME ( VK_OEM_CLEAR );
#undef ADD_NAME
    }
}
}
}
