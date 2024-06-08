#include "Stringify.hpp"
#include <stdexcept>

namespace CrossRenderer
{
namespace WindowManager
{
const char *Stringify ( const WindowEventType Value )
	{
#define STRINGIFY(X) case WindowEventType::X: return #X;
	switch ( Value )
		{
			STRINGIFY ( WindowCreated );
			STRINGIFY ( WindowClosed );
			STRINGIFY ( WindowMoved );
			STRINGIFY ( WindowResized );
			STRINGIFY ( WindowFramebufferResized );
			STRINGIFY ( WindowFocusChanged );
			STRINGIFY ( WindowStateChanged );
			STRINGIFY ( KeyPressed );
			STRINGIFY ( KeyReleased );
			STRINGIFY ( ButtonPressed );
			STRINGIFY ( ButtonReleased );
			STRINGIFY ( MouseWheel );
			STRINGIFY ( MouseMoved );
			STRINGIFY ( Quit );
			STRINGIFY ( TextInput );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled window event type" );
	}

const char *Stringify ( const WindowManagerBackend Value )
	{
#define STRINGIFY(X) case WindowManagerBackend::X: return #X;
	switch ( Value )
		{
			STRINGIFY ( GLFW3 );
		}
#undef STRINGIFY
	throw std::runtime_error ( "Unhandled window manager backend type" );
	}
}
}
