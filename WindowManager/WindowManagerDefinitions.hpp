#pragma once
#include "../RendererHandleTemplate.hpp"
#include "../GLMHeaders.hpp"
#include <string>

namespace CrossRenderer
{
namespace WindowManager
{
struct RenderWindowDescriptor
	{
	RenderWindowDescriptor ( const glm::ivec2 NewPosition = glm::ivec2 ( 100, 100 ), const glm::uvec2 NewSize = glm::uvec2 ( 1024, 768 ) )
		{
		Position = NewPosition;
		Size = NewSize;
		Resizable = true;
		Fullscreen = false;
		Title.assign ( "CrossRenderer" );
		}
	glm::uvec2 Size;
	glm::ivec2 Position;
	bool Resizable, Fullscreen;
	std::string Title;
	};

enum class WindowState
	{
	Minimized,
	Windowed,
	Maximized
	};

enum class KeyCode : uint32_t
	{
	Space = 32,
	Apostrophe = 39,
	Comma = 44,
	Minus = 45,
	Period = 46,
	Slash = 47,
	Number0 = 48,
	Number1 = 49,
	Number2 = 50,
	Number3 = 51,
	Number4 = 52,
	Number5 = 53,
	Number6 = 54,
	Number7 = 55,
	Number8 = 56,
	Number9 = 57,
	SemiColon = 59,
	Equal = 61,
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LeftBracket = 91,
	BackSlash = 92,
	RightBracket = 93,
	GraveAccent = 96,
	WORLD_1 = 161,
	WORLD_2 = 162,


	Escape = 256,
	Enter = 257,
	Tab = 258,
	Backspace = 259,
	Insert = 260,
	Delete = 261,
	Right = 262,
	Left = 263,
	Down = 264,
	Up = 265,
	PageUp = 266,
	PageDown = 267,
	Home = 268,
	End = 269,
	CapsLock = 280,
	ScrollLock = 281,
	NumLock = 282,
	PrintScreen = 283,
	Pause = 284,
	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,
	Keypad0 = 320,
	Keypad1 = 321,
	Keypad2 = 322,
	Keypad3 = 323,
	Keypad4 = 324,
	Keypad5 = 325,
	Keypad6 = 326,
	Keypad7 = 327,
	Keypad8 = 328,
	Keypad9 = 329,
	KeypadPeriod = 330,
	KeypadSlash = 331,
	KeypadAsterisk = 332,
	KeypadMinus = 333,
	KeypadPlus = 334,
	KeypadEnter = 335,
	KeypadEqual = 336,
	LeftShift = 340,
	LeftControl = 341,
	LeftAlt = 342,
	LeftSuper = 343,
	RightShift = 344,
	RightControl = 345,
	RightAlt = 346,
	RightSuper = 347,
	Menu = 348
	};

enum class WindowEventType
	{
	WindowCreated,
	WindowClosed,
	WindowMoved,
	WindowResized,
	WindowFocusChanged,
	WindowStateChanged,
	KeyPressed,
	KeyReleased,
	ButtonPressed,
	ButtonReleased,
	MouseWheel,
	MouseMoved,
	Quit,
	TextInput
	};

typedef struct WindowEvent
	{
	WindowEventType EventType;
	RenderWindowHandle OwnerHandle;
	struct
		{
		struct
			{
			KeyCode Key;
			uint32_t ScanCode;
			} KeyPressed, KeyReleased;
		struct
			{
			uint32_t Button;
			} ButtonPressed, ButtonReleased;
		struct
			{
			glm::vec2 Amount;
			} MouseWheel;
		struct
			{
			glm::ivec2 NewPosition, Delta;
			} MouseMoved;
		struct
			{
			uint32_t Data1, Data2;
			} Raw;
		struct
			{
			glm::ivec2 NewPosition;
			} WindowMoved;
		struct
			{
			glm::uvec2 NewSize;
			} WindowResized;
		struct
			{
			bool NewState;
			} WindowFocusChanged;
		struct
			{
			WindowState NewState;
			} WindowStateChanged;
		struct
			{
			char Text[32];
			} TextInput;
		void *Pointer;
		} EventData;
	WindowEvent ( void ) = default;
	} WindowEvent;

class EventListener
	{
	public:
		virtual ~EventListener ( void ) = default;
		virtual void OnEvent ( const WindowEvent &Event ) = 0;
	};

enum class WindowManagerBackend
	{
	GLFW3
	};
}
}
