#include "OpenGLRenderer.hpp"
#include "OpenGLInternals.hpp"
#include "../WindowManager/WindowManager.hpp"
#include "../Logger.hpp"
#include "../Renderer.hpp"
#include <algorithm>

namespace CrossRenderer
{
namespace OpenGL
{
RendererConfiguration Configuration;
unsigned GeneralVAO;
ShaderHandle ActiveShader;
RenderWindowHandle ActiveWindow;

bool DetectOpenGLInformation ( void );

bool InitializeRenderer ( const RendererConfiguration &NewConfiguration )
	{
	Configuration = NewConfiguration;
	for ( auto &Iterator : NewConfiguration.InitialWindowConfigurations )
		{
		RenderWindowHandle NewWindow = WindowManager::CreateNewWindow ( Iterator );
		if ( !NewWindow )
			return false;
		}
	ActiveWindow = *( WindowManager::WindowList.begin () );
	WindowManager::MakeGLActive ( ActiveWindow );
	gladLoadGL ();
	DetectOpenGLInformation ();

	if ( OpenGLInformation.OpenGLVersion.Lesser ( 3, 0 ) )
		{
		LOG_ERROR ( "OpenGL Core 3.0+ needed" );
		return false;
		}

	// If necessary, create a VAO
	glGenVertexArrays ( 1, &GeneralVAO );
	glBindVertexArray ( GeneralVAO );

	StateCache::SetDefaultViewportSize ( WindowManager::GetWindowSize(ActiveWindow) );
	StateCache::Invalidate ();

	glClearColor ( 0, 0, 0, 0 );
	glClearDepth ( 1.0 );
	glEnable ( GL_PROGRAM_POINT_SIZE );

	if ( CheckError () == false )
		goto OnError;

	LOG_DEBUG ( "Max Texture Units - %u", OpenGLInformation.MaxTextureUnits );

	glEnable ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback ( OpenGLMessageCallback, nullptr );
	return true;
OnError:
	if ( GeneralVAO != 0 )
		{
		glDeleteVertexArrays ( 1, &GeneralVAO );
		GeneralVAO = 0;
		}
	return false;
	}

bool ShutdownRenderer ( void )
	{
	if ( GeneralVAO != 0 )
		{
		glDeleteVertexArrays ( 1, &GeneralVAO );
		GeneralVAO = 0;
		}
	return true;
	}

bool StartRenderToFramebuffer ( const FramebufferHandle &Handle )
	{
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey()];

	StateCache::SetDefaultViewportSize ( FramebufferInformation->Dimensions );
	StateCache::ConfigureScissor ( ScissorSettings () );
	StateCache::ConfigureViewport ( ViewportSettings () );
	StateCache::ConfigureStencil ( StencilBufferSettings () );
	StateCache::ConfigureFramebuffer ( Handle );
	int BitMask = 0;
	if ( FramebufferInformation->ColorTextures.size () )
		{
		BitMask |= GL_COLOR_BUFFER_BIT;
		glClearColor ( FramebufferInformation->ClearColor.r, FramebufferInformation->ClearColor.g, FramebufferInformation->ClearColor.b, FramebufferInformation->ClearColor.a );
		}
	if ( FramebufferInformation->DepthTexture )
		{
		BitMask |= GL_DEPTH_BUFFER_BIT;
		glClearDepth ( FramebufferInformation->ClearDepth );
		}
	if ( BitMask )
		glClear ( BitMask );
	return CheckError ();
	}

bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle )
	{
	ActiveWindow = WindowHandle;
	WindowManager::MakeGLActive ( WindowHandle );
	glm::uvec2 WindowSize = WindowManager::GetWindowSize ( WindowHandle );
	StateCache::SetDefaultViewportSize ( WindowSize );
	StateCache::ConfigureScissor ( ScissorSettings () );
	StateCache::ConfigureViewport ( ViewportSettings () );
	StateCache::ConfigureStencil ( StencilBufferSettings () );

	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey()];
	if ( FramebufferInformation->OpenGLID != 0 )
		{
		glBindFramebuffer ( GL_READ_FRAMEBUFFER, FramebufferInformation->OpenGLID );
		glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, 0 );
		glBlitFramebuffer (
			0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
			0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
			GL_COLOR_BUFFER_BIT, GL_NEAREST );
		// Restore old framebuffer binding
		glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, FramebufferInformation->OpenGLID );
		}
	return CheckError ();
	}

bool StartFrame ( const RenderWindowHandle &WindowHandle )
	{
	ActiveWindow = WindowHandle;
	StateCache::SetDefaultViewportSize ( WindowManager::GetWindowSize ( WindowHandle ) );
	StateCache::ConfigureScissor ( ScissorSettings () );
	StateCache::ConfigureViewport ( ViewportSettings () );
	StateCache::ConfigureStencil ( StencilBufferSettings () );

	WindowManager::MakeGLActive ( WindowHandle );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	return CheckError ();
	}

bool EndFrame ( const RenderWindowHandle &WindowHandle )
	{
	StateCache::ConfigureScissor (ScissorSettings ());
	StateCache::ConfigureViewport ( ViewportSettings () );
	StateCache::ConfigureStencil ( StencilBufferSettings () );
	WindowManager::SwapGLWindowBuffer ( WindowHandle );
	return CheckError ();
	}

bool RunCommand ( const RenderCommand &Command )
	{
	if ( !Command.Shader )
		return false;
	if ( !Command.VertexCount )
		return false;

	// Disable active vertex attributes
	if ( ( ActiveShader ) && ( ActiveShader != Command.Shader ) )
		{
		ShaderInfo *OldShaderInfo = &Shaders[ActiveShader.GetKey ()];
		for ( auto &AttributeIterator : OldShaderInfo->Attributes )
			{
			if ( AttributeIterator.Enabled == true )
				{
				AttributeIterator.Enabled = false;
				glDisableVertexAttribArray ( AttributeIterator.OpenGLID );
				}
			}
		}

	// Activate new shader
	ShaderInfo *ShaderInformation = &Shaders[Command.Shader.GetKey ()];
	if ( ActiveShader != Command.Shader )
		{
		glUseProgram ( ShaderInformation->OpenGLID );
		ActiveShader = Command.Shader;
		if ( CheckError () == false )
			return false;
		}

	StateCache::ApplyState ( Command.State );
	if ( CheckError () == false )
		return false;

	// Set all uniforms
	unsigned UniformBlockBindingIndex = 0;
	for ( auto &Iterator : Command.UniformValues )
		{
		if ( !Iterator.UniformHandle )
			return false;
		UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.GetKey ()];

		if ( UniformInformation->CurrentValue.Equals ( Iterator.UniformValue, UniformInformation->Type ) == true )
			continue;

		switch ( UniformInformation->Type )
			{
#define CASE_TYPE(TYPENAME,FUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( UniformInformation->OpenGLID, Iterator.UniformValue.TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
            FUNCTION ( UniformInformation->OpenGLID, 1, glm::value_ptr(Iterator.UniformValue.TYPENAME##Value) );\
            break;\
            }

			CASE_TYPE ( Float, glUniform1f );
			CASE_TYPE_VEC ( Float2, glUniform2fv );
			CASE_TYPE_VEC ( Float3, glUniform3fv );
			CASE_TYPE_VEC ( Float4, glUniform4fv );

			CASE_TYPE ( Integer, glUniform1i );
			CASE_TYPE_VEC ( Integer2, glUniform2iv );
			CASE_TYPE_VEC ( Integer3, glUniform3iv );
			CASE_TYPE_VEC ( Integer4, glUniform4iv );

			CASE_TYPE ( UnsignedInteger, glUniform1ui );
			CASE_TYPE_VEC ( UnsignedInteger2, glUniform2uiv );
			CASE_TYPE_VEC ( UnsignedInteger3, glUniform3uiv );
			CASE_TYPE_VEC ( UnsignedInteger4, glUniform4uiv );
#undef CASE_TYPE
#undef CASE_TYPE_VEC

			case ShaderUniformType::Matrix3:
				glUniformMatrix3fv ( UniformInformation->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Iterator.UniformValue.Matrix3Value ) );
				break;
			case ShaderUniformType::Matrix4:
				glUniformMatrix4fv ( UniformInformation->OpenGLID, 1, GL_FALSE, glm::value_ptr ( Iterator.UniformValue.Matrix4Value ) );
				break;
			case ShaderUniformType::Bool:
				glUniform1i ( UniformInformation->OpenGLID, Iterator.UniformValue.BoolValue );
				break;
			case ShaderUniformType::Block:
				{
				ShaderBufferInfo SBInfo = ShaderBuffers[Iterator.UniformValue.BlockValue.GetKey()];
				glUniformBlockBinding ( ShaderInformation->OpenGLID, UniformInformation->OpenGLID, UniformBlockBindingIndex );
				glBindBufferRange ( GL_UNIFORM_BUFFER, UniformBlockBindingIndex, SBInfo.OpenGLID, 0, SBInfo.DataSize );
				++UniformBlockBindingIndex;
				break;
				}
			default:
				throw std::runtime_error ( "Unhandled shader uniform type" );
			}
		UniformInformation->CurrentValue = Iterator.UniformValue;
		}
	if ( CheckError () == false )
		return false;

	// Bind all attribute buffers
	for ( auto &Iterator : Command.ShaderBufferBindings )
		{
		AttributeInfo *AttributeInformation = &( ShaderInformation->Attributes[Iterator.AttributeHandle.GetKey ()] );
		ShaderBufferInfo *BufferInformation = &( ShaderBuffers[Iterator.DataStream.BufferHandle.GetKey ()] );

		if ( AttributeInformation->Enabled == false )
			{
			glEnableVertexAttribArray ( AttributeInformation->OpenGLID );
			AttributeInformation->Enabled = true;
			}

		if ( BufferInformation->MappedPointer )
			{
			UnmapShaderBuffer ( Iterator.DataStream.BufferHandle );
			}

		glBindBuffer ( GL_ARRAY_BUFFER, BufferInformation->OpenGLID );
		glVertexAttribPointer ( AttributeInformation->OpenGLID,
								(GLint) Iterator.DataStream.ComponentsPerElement,
								Translate ( Iterator.DataStream.ComponentType ),
								Iterator.DataStream.NormalizeData,
								(GLsizei) Iterator.DataStream.Stride,
								(void *) Iterator.DataStream.StartOffset );
		}
	if ( CheckError () == false )
		return false;

	// Bind all textures
	unsigned TextureBindingIndex = 0;
	for ( auto &Iterator : Command.TextureBindings )
		{
		UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.GetKey ()];

		GLint DesiredSWrap = Translate ( Iterator.BindSettings.WrapSettings.Horizontal );
		GLint DesiredTWrap = Translate ( Iterator.BindSettings.WrapSettings.Vertical );
		GLint DesiredMinFilter = Translate ( Iterator.BindSettings.FilterSettings.MinFilter );
		GLint DesiredMagFilter = Translate ( Iterator.BindSettings.FilterSettings.MagFilter );
		TextureInfo *TextureInformation = &Textures[Iterator.BindSettings.Handle.GetKey ()];

		GLint TextureType;
		switch ( TextureInformation->Type )
			{
			case TextureType::Texture2D:
				{
				TextureType = GL_TEXTURE_2D;
				break;
				}
			case TextureType::TextureCubeMap:
				{
				TextureType = GL_TEXTURE_CUBE_MAP;
				break;
				}
			default:
				return false;
			}

		glActiveTexture ( GL_TEXTURE0 + TextureBindingIndex ); // Multitexture index
		glBindTexture ( TextureType, TextureInformation->OpenGLID );
		if ( TextureInformation->GLSWrap != DesiredSWrap )
			glTexParameteri ( TextureType, GL_TEXTURE_WRAP_S, DesiredSWrap );
		if ( TextureInformation->GLTWrap != DesiredTWrap )
			glTexParameteri ( TextureType, GL_TEXTURE_WRAP_T, DesiredTWrap );
		if ( TextureInformation->GLMinFilter != DesiredMinFilter )
			glTexParameteri ( TextureType, GL_TEXTURE_MIN_FILTER, DesiredMinFilter );
		if ( TextureInformation->GLMagFilter != DesiredMagFilter )
			glTexParameteri ( TextureType, GL_TEXTURE_MAG_FILTER, DesiredMagFilter );
		glUniform1i ( UniformInformation->OpenGLID, (GLint) TextureBindingIndex );

		if ( CheckError () == false )
			{
			LOG_ERROR ( "Error activating texture" );
			return false;
			}
		TextureInformation->GLSWrap = DesiredSWrap;
		TextureInformation->GLTWrap = DesiredTWrap;
		TextureInformation->GLMinFilter = DesiredMinFilter;
		TextureInformation->GLMagFilter = DesiredMagFilter;

		++TextureBindingIndex;
		}
	if ( CheckError () == false )
		return false;

	// Finally, issue the draw call
	if ( Command.IndexBufferStream.BufferHandle )
		{
		static size_t Sizes[] = { sizeof ( float ), sizeof ( uint8_t ), sizeof ( uint16_t ), sizeof ( uint32_t ) };

		ShaderBufferInfo *BufferToUse = &ShaderBuffers[Command.IndexBufferStream.BufferHandle.GetKey ()];
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, BufferToUse->OpenGLID );
		glDrawElements ( Translate ( Command.Primitive ),
						 (GLint) Command.VertexCount,
						 Translate ( Command.IndexBufferStream.ComponentType ),
						 reinterpret_cast <void *> ( Command.StartVertex * Sizes[(int) Command.IndexBufferStream.ComponentType] ) );
		}
	else
		{
		glDrawArrays ( Translate ( Command.Primitive ), (GLint) Command.StartVertex, static_cast <GLsizei> ( Command.VertexCount ) );
		}
	return CheckError ();
	}


bool DetectOpenGLInformation ( void )
	{
	// MAJOR & MINOR only introduced in GL {,ES} 3.0
	GLint TempMajor = 0, TempMinor = 0;
	glGetIntegerv ( GL_MAJOR_VERSION, &TempMajor );
	glGetIntegerv ( GL_MINOR_VERSION, &TempMinor );
	if ( ( glGetError () == GL_NO_ERROR ) && ( TempMajor != 0 ) )
		{
		OpenGLInformation.OpenGLVersion.Set ( static_cast <unsigned> ( TempMajor ), static_cast <unsigned> ( TempMinor ) );
		}
	else
		{
		// Still don't have the version. Try to get it with GL_VERSION...
		const GLubyte *GLVersionString = glGetString ( GL_VERSION );
		if ( GLVersionString == nullptr )
			return false;
		// < v3.0; resort to the old-fashioned way.
		LOG_DEBUG ( "Reported GL version string : '%s'", GLVersionString );
		if ( OpenGLInformation.OpenGLVersion.ParseString ( reinterpret_cast <const char *> ( GLVersionString ) ) == false )
			return false;
		}
	LOG_DEBUG ( "OpenGL version %d.%d", OpenGLInformation.OpenGLVersion.Major, OpenGLInformation.OpenGLVersion.Minor );

	/**
	OpenGL  #version    latest spec version number  Note
	2.0     100     1.10.59
	2.1     120     1.20.8
	3.0     130     1.30.10
	3.1     140     1.40.08
	3.2     150     1.50.11     added Geometry Shaders
	3.3     330     3.30.6
	4.0     400     4.00.9  added both Tessellation Shaders
	4.1     410     4.10.6
	4.2     420     4.20.11
	4.3     430     4.30.6   added Compute Shaders
	*/

	// Safe assumptions about GLSL
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 2, 0 ) ) OpenGLInformation.GLSLVersion.Set ( 1, 0 );
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 2, 1 ) ) OpenGLInformation.GLSLVersion.Set ( 1, 2 );
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 3, 0 ) ) OpenGLInformation.GLSLVersion.Set ( 1, 3 );
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 3, 1 ) ) OpenGLInformation.GLSLVersion.Set ( 1, 4 );
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 3, 2 ) ) OpenGLInformation.GLSLVersion.Set ( 1, 5 );
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 3, 3 ) ) OpenGLInformation.GLSLVersion = OpenGLInformation.OpenGLVersion;

	const GLubyte *GLSLVersionString = glGetString ( GL_SHADING_LANGUAGE_VERSION );
	if ( GLSLVersionString != nullptr )
		{
		LOG_DEBUG ( "Reported GLSL version string : '%s'", GLSLVersionString );
		if ( OpenGLInformation.GLSLVersion.ParseString ( reinterpret_cast <const char *> ( GLSLVersionString ) ) == false )
			LOG_ERROR ( "Error parsing GLSL version string. Assuming safe values" );
		}
	LOG_DEBUG ( "GLSL version %d.%d", OpenGLInformation.GLSLVersion.Major, OpenGLInformation.GLSLVersion.Minor );

	const char *String = reinterpret_cast<const char *> ( glGetString ( GL_RENDERER ) );
	if ( ( String != nullptr ) && ( strlen ( String ) > 0 ) )
		OpenGLInformation.OpenGLRendererString.assign ( String );

	LOG_DEBUG ( "OpenGL renderer string : '%s'", OpenGLInformation.OpenGLRendererString.c_str () );
	for ( GLenum iterator = 0; iterator < /*std::numeric_limits<unsigned>::max ()*/3; ++iterator )
		{
		String = reinterpret_cast<const char *> ( glGetString ( GL_VENDOR + iterator ) );
		if ( ( String != nullptr ) && ( strlen ( String ) > 0 ) )
			{
			if ( OpenGLInformation.OpenGLVendorString.length () > 0 )
				OpenGLInformation.OpenGLVendorString.append ( "\n" );
			OpenGLInformation.OpenGLVendorString.append ( String );
			}
		else
			break;
		}
	LOG_DEBUG ( "OpenGL vendor string : '%s'", OpenGLInformation.OpenGLVendorString.c_str () );

	OpenGLInformation.Extensions.clear ();
	// Method 1 - glGetStringi is only guaranteed to be available after glcore/gles 3.0
	if ( OpenGLInformation.OpenGLVersion.GreaterEqual ( 3, 0 ) )
		{
		// Try to get extensions using the index
		int ExtensionCount = 0;
		glGetIntegerv ( GL_NUM_EXTENSIONS, &ExtensionCount );
		if ( ( glGetError () == GL_NO_ERROR ) && ( ExtensionCount != 0 ) && ( glGetStringi != nullptr ) )
			{
			OpenGLInformation.Extensions.reserve ( ExtensionCount );
			for ( int ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ++ExtensionIndex )
				{
				const char *CurrentExtension = (const char *) glGetStringi ( GL_EXTENSIONS, ExtensionIndex );
				if ( CurrentExtension == nullptr ) continue;
				OpenGLInformation.Extensions.push_back ( CurrentExtension );
				}
			}
		}
	// Method 2
	// Profiles >= 3.0 are not guaranteed to have GL_EXTENSIONS support for glGetString
	if ( OpenGLInformation.Extensions.empty () )
		{
		const char *ExtensionString, *Begin, *End;
		ExtensionString = (const char *) glGetString ( GL_EXTENSIONS );
		if ( ExtensionString == NULL )
			return false;
		if ( glGetError () == GL_NO_ERROR )
			{
			unsigned Length = (unsigned) strlen ( ExtensionString );
			for ( Begin = ExtensionString; Begin < ExtensionString + Length; Begin = End + 1 )
				{
				End = strchr ( Begin, ' ' );
				if ( End == NULL ) End = ExtensionString + Length;

				std::string NewExtension;
				NewExtension.assign ( Begin, End );
				OpenGLInformation.Extensions.push_back ( NewExtension );
				}
			}
		}
	std::sort ( OpenGLInformation.Extensions.begin (), OpenGLInformation.Extensions.end () );
#if 1
	LOG_DEBUG ( "Detected extensions: %lu", (unsigned long) OpenGLInformation.Extensions.size () );
	for ( const auto &Iterator : OpenGLInformation.Extensions )
		LOG_DEBUG ( "%s", Iterator.c_str () );
#endif

	glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &OpenGLInformation.MaxTextureUnits );

	return true;
	}
}
}
