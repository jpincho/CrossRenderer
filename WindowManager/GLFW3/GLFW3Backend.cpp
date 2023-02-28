#include "GLFW3Backend.hpp"
#include <GLFW/glfw3.h>
#include "../../Internal/Logger.hpp"
#include "../../Internal/VectorizedContainer.hpp"

namespace CrossRenderer
{
namespace WindowManager
{
namespace GLFW3
{
struct GLFW3WindowInfo
	{
	GLFWwindow *Window;
	glm::ivec2 Position;
	glm::uvec2 Size;
	std::string Title;
	WindowState State;
	};

static VectorizedContainer <GLFW3WindowInfo> Windows;
static glm::ivec2 MousePosition = glm::ivec2 ( 0, 0 );
static uint32_t MouseButtons = 0;

void GLFWWindowPosCallback ( GLFWwindow *GLFWHandle, int NewX, int NewY );
void GLFWWindowSizeCallback ( GLFWwindow *GLFWHandle, int NewX, int NewY );
void GLFWWindowCloseCallback ( GLFWwindow *GLFWHandle );
void GLFWWindowFocusCallback ( GLFWwindow *GLFWHandle, int State );
void GLFWWindowIconifyCallback ( GLFWwindow *GLFWHandle, int State );
void GLFWWindowMaximizeCallback ( GLFWwindow *GLFWHandle, int State );
void GLFWKeyCallback ( GLFWwindow *GLFWHandle, int Key, int Scancode, int Action, int );
void GLFWMouseButtonCallback ( GLFWwindow *GLFWHandle, int Button, int Action, int );
void GLFWMouseWheelCallback ( GLFWwindow *GLFWHandle, double XOffset, double YOffset );
void GLFWMouseCursorCallback ( GLFWwindow *GLFWHandle, double XOffset, double YOffset );
void GLFWCharCallback ( GLFWwindow *GLFWHandle, unsigned int Char );
void GLFWErrorCallback ( int Error, const char *Description );

RenderWindowHandle CreateNewWindow ( const RenderWindowDescriptor &Descriptor )
	{
	if ( !glfwInit () )
		return RenderWindowHandle::Invalid;

	GLFW3WindowInfo NewWindow;
	NewWindow.Title = Descriptor.Title;

	glfwWindowHint ( GLFW_RESIZABLE, Descriptor.Resizable ? GLFW_TRUE : GLFW_FALSE );

	NewWindow.Window = glfwCreateWindow ( Descriptor.Size.x, Descriptor.Size.y, Descriptor.Title.c_str (), nullptr, nullptr );
	if ( NewWindow.Window == nullptr )
		return RenderWindowHandle::Invalid;
	glfwSetWindowPos ( NewWindow.Window, Descriptor.Position.x, Descriptor.Position.y );

	glfwGetWindowSize ( NewWindow.Window, (int *) &NewWindow.Size.x, (int *) &NewWindow.Size.y );
	glfwGetWindowPos ( NewWindow.Window, &NewWindow.Position.x, &NewWindow.Position.y );

	double TempMouseX, TempMouseY;
	glfwGetCursorPos ( NewWindow.Window, &TempMouseX, &TempMouseY );
	MousePosition.x = (int) floor ( TempMouseX );
	MousePosition.y = (int) floor ( TempMouseY );
	MouseButtons = 0;
	for ( unsigned Button = 0; Button < 8; ++Button )
		{
		uint32_t Mask = 1 << Button;
		int State = glfwGetMouseButton ( NewWindow.Window, Button );
		if ( State == GLFW_PRESS )
			MouseButtons |= Mask;
		}
	glfwSetWindowCloseCallback ( NewWindow.Window, GLFWWindowCloseCallback );
	glfwSetWindowPosCallback ( NewWindow.Window, GLFWWindowPosCallback );
	glfwSetWindowSizeCallback ( NewWindow.Window, GLFWWindowSizeCallback );
	glfwSetWindowFocusCallback ( NewWindow.Window, GLFWWindowFocusCallback );
	glfwSetWindowIconifyCallback ( NewWindow.Window, GLFWWindowIconifyCallback );
	glfwSetWindowMaximizeCallback ( NewWindow.Window, GLFWWindowMaximizeCallback );
	glfwSetKeyCallback ( NewWindow.Window, GLFWKeyCallback );
	glfwSetMouseButtonCallback ( NewWindow.Window, GLFWMouseButtonCallback );
	glfwSetScrollCallback ( NewWindow.Window, GLFWMouseWheelCallback );
	glfwSetCursorPosCallback ( NewWindow.Window, GLFWMouseCursorCallback );
	glfwSetCharCallback ( NewWindow.Window, GLFWCharCallback );
	if ( Descriptor.Fullscreen )
		{
		glfwMaximizeWindow ( NewWindow.Window );
		NewWindow.State = WindowState::Maximized;
		}
	else
		NewWindow.State = WindowState::Windowed;

	LOG_DEBUG ( "Finished creating window %dx%d with size %ux%u. Title '%s'", NewWindow.Position.x, NewWindow.Position.y, NewWindow.Size.x, NewWindow.Size.y, NewWindow.Title.c_str () );

	glfwMakeContextCurrent ( NewWindow.Window );
	glfwSwapInterval ( 1 );

	RenderWindowHandle NewHandle ( Windows.GetFreeIndex () );
	glfwSetWindowUserPointer ( NewWindow.Window, (void *) NewHandle.GetKey () );
	Windows[NewHandle.GetKey()] = NewWindow;
	WindowManager::WindowList.insert ( NewHandle );
	return NewHandle;
	}

bool DestroyWindow ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey()];
	glfwDestroyWindow ( WindowInformation->Window );
	Windows.ReleaseIndex ( Handle.GetKey () );
	WindowManager::WindowList.erase ( Handle );
	return true;
	}

void SetWindowPosition ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	glfwSetWindowPos ( WindowInformation->Window, NewPosition.x, NewPosition.y );
	glfwGetWindowPos ( WindowInformation->Window, &WindowInformation->Position.x, &WindowInformation->Position.y );
	}

glm::ivec2 GetWindowPosition ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	return WindowInformation->Position;
	}

void SetWindowSize ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	glfwSetWindowSize ( WindowInformation->Window, NewSize.x, NewSize.y );
	glfwGetWindowSize ( WindowInformation->Window, (int *) &WindowInformation->Size.x, (int *) &WindowInformation->Size.y );
	}

glm::uvec2 GetWindowSize ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	return WindowInformation->Size;
	}

void SetWindowTitle ( const RenderWindowHandle &Handle, const char *NewTitle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	glfwSetWindowTitle ( WindowInformation->Window, NewTitle );
	WindowInformation->Title.assign ( NewTitle );
	}

const char *GetWindowTitle ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	return WindowInformation->Title.c_str ();
	}

bool SetWindowState ( const RenderWindowHandle &Handle, const WindowState NewState )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];

	if ( WindowInformation->State == NewState )
		return true;
	switch ( NewState )
		{
		case WindowState::Minimized:
			{
			glfwIconifyWindow ( WindowInformation->Window );
			break;
			}
		case WindowState::Windowed:
			{
			glfwRestoreWindow ( WindowInformation->Window );
			break;
			}
		case WindowState::Maximized:
			{
			glfwMaximizeWindow ( WindowInformation->Window );
			break;
			}
		default:
			return false;
		}
	glfwGetWindowSize ( WindowInformation->Window, (int *) &WindowInformation->Size.x, (int *) &WindowInformation->Size.y );
	glfwGetWindowPos ( WindowInformation->Window, &WindowInformation->Position.x, &WindowInformation->Position.y );
	WindowInformation->State = NewState;
	return true;
	}

WindowState GetWindowState ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	return WindowInformation->State;
	}

void SetMouseCursorState ( const RenderWindowHandle &Handle, const bool NewState )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	glfwSetInputMode ( WindowInformation->Window, GLFW_CURSOR, NewState == true ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED );
	}

bool GetMouseCursorState ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	int State = glfwGetInputMode ( WindowInformation->Window, GLFW_CURSOR );
	if ( State == GLFW_CURSOR_DISABLED )
		return false;
	return true;
	}

glm::ivec2 GetMousePosition ( void )
	{
	return MousePosition;
	}

uint32_t GetMouseButtonStatus ( void )
	{
	return MouseButtons;
	}

void MakeGLActive ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	glfwMakeContextCurrent ( WindowInformation->Window );
	}

void SwapGLWindowBuffer ( const RenderWindowHandle &Handle )
	{
	GLFW3WindowInfo *WindowInformation = &Windows[Handle.GetKey ()];
	glfwSwapBuffers ( WindowInformation->Window );
	glfwPollEvents ();
	}

//***************************************************************************
//* Callbacks
//***************************************************************************
void GLFWWindowPosCallback ( GLFWwindow *GLFWHandle, int NewX, int NewY )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	GLFW3WindowInfo *WindowInformation = &Windows[Handle];
	WindowInformation->Position.x = NewX;
	WindowInformation->Position.y = NewY;

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::WindowMoved;
	NewEvent.EventData.WindowMoved.NewPosition = WindowInformation->Position;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWWindowSizeCallback ( GLFWwindow *GLFWHandle, int NewX, int NewY )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	GLFW3WindowInfo *WindowInformation = &Windows[Handle];
	WindowInformation->Size.x = NewX;
	WindowInformation->Size.y = NewY;

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::WindowResized;
	NewEvent.EventData.WindowResized.NewSize = WindowInformation->Size;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWWindowCloseCallback ( GLFWwindow *GLFWHandle )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::WindowClosed;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWWindowFocusCallback ( GLFWwindow *GLFWHandle, int State )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::WindowFocusChanged;
	NewEvent.EventData.WindowFocusChanged.NewState = State;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWWindowIconifyCallback ( GLFWwindow *GLFWHandle, int State )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	GLFW3WindowInfo *WindowInformation = &Windows[Handle];

	if ( State )
		WindowInformation->State = WindowState::Minimized;
	else
		WindowInformation->State = WindowState::Windowed;
	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::WindowStateChanged;
	NewEvent.EventData.WindowStateChanged.NewState = WindowInformation->State;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWWindowMaximizeCallback ( GLFWwindow *GLFWHandle, int State )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	GLFW3WindowInfo *WindowInformation = &Windows[Handle];
	if ( State )
		WindowInformation->State = WindowState::Maximized;
	else
		WindowInformation->State = WindowState::Windowed;

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::WindowStateChanged;
	NewEvent.EventData.WindowStateChanged.NewState = WindowInformation->State;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWKeyCallback ( GLFWwindow *GLFWHandle, int Key, int Scancode, int Action, int )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	switch ( Action )
		{
		case GLFW_PRESS:
			NewEvent.EventType = WindowEventType::KeyPressed;
			NewEvent.EventData.KeyPressed.Key = (KeyCode) Key;
			NewEvent.EventData.KeyPressed.ScanCode = Scancode;
			break;
		case GLFW_RELEASE:
			NewEvent.EventType = WindowEventType::KeyReleased;
			NewEvent.EventData.KeyReleased.Key = (KeyCode) Key;
			NewEvent.EventData.KeyReleased.ScanCode = Scancode;
			break;
		default:
			return;
		}
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWMouseButtonCallback ( GLFWwindow *GLFWHandle, int Button, int Action, int )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	switch ( Action )
		{
		case GLFW_PRESS:
			NewEvent.EventType = WindowEventType::ButtonPressed;
			NewEvent.EventData.ButtonPressed.Button = Button;
			MouseButtons |= 1 << Button;
			break;
		case GLFW_RELEASE:
			NewEvent.EventType = WindowEventType::ButtonReleased;
			NewEvent.EventData.ButtonReleased.Button = Button;
			MouseButtons ^= 1 << Button;
			break;
		default:
			return;
		}
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWMouseWheelCallback ( GLFWwindow *GLFWHandle, double XOffset, double YOffset )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::MouseWheel;
	NewEvent.EventData.MouseWheel.Amount.x = (float) XOffset;
	NewEvent.EventData.MouseWheel.Amount.y = (float) YOffset;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWMouseCursorCallback ( GLFWwindow *GLFWHandle, double XOffset, double YOffset )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::MouseMoved;
	NewEvent.EventData.MouseMoved.NewPosition.x = (int) floor ( XOffset );
	NewEvent.EventData.MouseMoved.NewPosition.y = (int) floor ( YOffset );
	NewEvent.EventData.MouseMoved.Delta.x = NewEvent.EventData.MouseMoved.NewPosition.x - MousePosition.x;
	NewEvent.EventData.MouseMoved.Delta.y = NewEvent.EventData.MouseMoved.NewPosition.y - MousePosition.y;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	MousePosition = NewEvent.EventData.MouseMoved.NewPosition;
	SendWindowEvent ( NewEvent );
	}

void GLFWCharCallback ( GLFWwindow *GLFWHandle, unsigned int Char )
	{
	intptr_t Handle = (intptr_t) glfwGetWindowUserPointer ( GLFWHandle );

	WindowEvent NewEvent;
	NewEvent.EventType = WindowEventType::TextInput;
	NewEvent.EventData.TextInput.Text[0] = (char) Char;
	NewEvent.EventData.TextInput.Text[1] = 0;
	NewEvent.OwnerHandle = RenderWindowHandle ( Handle );
	SendWindowEvent ( NewEvent );
	}

void GLFWErrorCallback ( int Error, const char *Description )
	{
	LOG_ERROR ( "GLFW error %02X '%s'", Error, Description );
	}
}
}
}
