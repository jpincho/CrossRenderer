#include <CrossRendererConfig.hpp>
#include "TestBase.hpp"
#include "../Internal/Logger.hpp"
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#if ( defined CROSS_RENDERER_COMPILER_GNU )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <stb/stb_image.h>
#pragma GCC diagnostic pop
#elif ( defined CROSS_RENDERER_COMPILER_MSVC )
#include <stb/stb_image.h>
#endif

bool TestBase::Initialize ( void )
	{
	ShouldQuit = true;
	CrossRenderer::WindowManager::AddEventListener ( this );

	// CrossRenderer initialization
	CrossRenderer::RendererConfiguration NewConfiguration;
	NewConfiguration.DesiredRendererBackend = CrossRenderer::GetDefaultRendererBackendType ();
	NewConfiguration.InitialWindowConfigurations.push_back ( CrossRenderer::WindowManager::RenderWindowDescriptor ( glm::ivec2 ( 100, 100 ), glm::uvec2 ( 1920, 1080 ) ) );
	NewConfiguration.DesiredWindowBackend = CrossRenderer::GetDefaultWindowManagerBackendType ();
	if ( CrossRenderer::Initialize ( NewConfiguration ) == false )
		return false;

	CrossRenderer::FramebufferDescriptor NewFramebufferDescriptor ( glm::uvec2 ( 1920, 1080 ) );
	Framebuffer = CrossRenderer::CreateFramebuffer ( NewFramebufferDescriptor );
	if ( !Framebuffer )
		return false;
	ShouldQuit = false;
	return SpecificInitialize ();
	}

bool TestBase::Shutdown ( void )
	{
	SpecificShutdown ();
	CrossRenderer::DeleteFramebuffer ( Framebuffer );
	CrossRenderer::Shutdown ();
	return true;
	}

bool TestBase::Run ( void )
	{
	const float TimeDelta = 1.0f / 60;

	while ( ShouldQuit == false )
		{
		CrossRenderer::StartRenderToFramebuffer ( Framebuffer );

		if ( SpecificFrame ( TimeDelta ) == false )
			{
			ShouldQuit = true;
			break;
			}
		CrossRenderer::DisplayFramebuffer ( Framebuffer, *CrossRenderer::WindowManager::WindowList.begin () );
		CrossRenderer::EndFrame ( *CrossRenderer::WindowManager::WindowList.begin () );
		}
	return true;
	}

void TestBase::OnEvent ( const CrossRenderer::WindowManager::WindowEvent &Event )
	{
	switch ( Event.EventType )
		{
		case CrossRenderer::WindowManager::WindowEventType::KeyPressed:
			{
			if ( Event.EventData.KeyPressed.Key == CrossRenderer::WindowManager::KeyCode::Escape )
				{
				ShouldQuit = true;
				}
			break;
			}
		case CrossRenderer::WindowManager::WindowEventType::WindowResized:
			{
			CrossRenderer::DeleteFramebuffer ( Framebuffer );
			CrossRenderer::FramebufferDescriptor NewFramebufferDescriptor ( Event.EventData.WindowResized.NewSize );
			Framebuffer = CrossRenderer::CreateFramebuffer ( NewFramebufferDescriptor );
			break;
			}
		case CrossRenderer::WindowManager::WindowEventType::WindowClosed:
			{
			CrossRenderer::WindowManager::DestroyWindow ( Event.OwnerHandle );
			if ( CrossRenderer::WindowManager::WindowList.size () == 0 )
				{
				ShouldQuit = true;
				}
			break;
			}
		default:
			break;
		}
	SpecificOnEvent ( Event );
	}

CrossRenderer::ShaderHandle LoadShader ( const std::string &VertexFile, const std::string &GeometryFile, const std::string &FragmentFile )
	{
	std::vector <uint8_t> VShaderFileContents, GShaderFileContents, FShaderFileContents;
	// Load the shader
	if ( LoadFileContents ( VertexFile, VShaderFileContents ) == false )
		{
		LOG_ERROR ( "Unable to load vertex shader from '%s'", VertexFile.c_str () );
		return CrossRenderer::ShaderHandle::Invalid;
		}

	if ( ( GeometryFile.length () ) && ( LoadFileContents ( GeometryFile, GShaderFileContents ) == false ) )
		{
		LOG_ERROR ( "Unable to load geometry shader from '%s'", GeometryFile.c_str () );
		return CrossRenderer::ShaderHandle::Invalid;
		}
	if ( LoadFileContents ( FragmentFile, FShaderFileContents ) == false )
		{
		LOG_ERROR ( "Unable to load fragment shader from '%s'", FragmentFile.c_str () );
		return CrossRenderer::ShaderHandle::Invalid;
		}

	CrossRenderer::ShaderCode ShaderCode;
	ShaderCode.VertexShader.assign ( (const char *) VShaderFileContents.data (), (const char *) VShaderFileContents.data () + VShaderFileContents.size () );
	if ( GeometryFile.length () )
		ShaderCode.GeometryShader.assign ( (const char *) GShaderFileContents.data (), (const char *) GShaderFileContents.data () + GShaderFileContents.size () );
	ShaderCode.FragmentShader.assign ( (const char *) FShaderFileContents.data (), (const char *) FShaderFileContents.data () + FShaderFileContents.size () );
	CrossRenderer::ShaderHandle Shader = CrossRenderer::CreateShader ( ShaderCode );
	if ( !Shader )
		{
		LOG_ERROR ( "Unable to build shader from '%s' and '%s'", VertexFile.c_str (), FragmentFile.c_str () );
		}
	return Shader;
	}

bool LoadFileContents ( const std::string &Path, std::vector <uint8_t> &Contents )
	{
	std::streampos size;

	std::ifstream file ( Path.c_str (), std::ios::in | std::ios::binary | std::ios::ate );
	if ( !file.is_open () )
		return false;

	size = file.tellg ();
	Contents.resize ( size );
	file.seekg ( 0, std::ios::beg );
	file.read ( (char *) Contents.data (), size );
	file.close ();
	return true;
	}

CrossRenderer::TextureHandle LoadTexture ( const std::string &ImageFile, const bool Flip )
	{
	glm::ivec2 ImageSize;
	int Channels;
	CrossRenderer::PixelFormat ImageFormat;

	stbi_set_flip_vertically_on_load ( Flip ? 1 : 0 );
	stbi_uc *Image = stbi_load ( ImageFile.c_str (), &ImageSize.x, &ImageSize.y, &Channels, 0 );
	if ( !Image )
		{
		LOG_ERROR ( "Unable to load texture from '%s'", ImageFile.c_str () );
		return CrossRenderer::TextureHandle::Invalid;
		}
	switch ( Channels )
		{
		case 3:
			ImageFormat = CrossRenderer::PixelFormat::RedGreenBlue888;
			break;
		case 4:
			ImageFormat = CrossRenderer::PixelFormat::RedGreenBlueAlpha8888;
			break;
		default:
			stbi_image_free ( Image );
			return CrossRenderer::TextureHandle::Invalid;
		}

	CrossRenderer::TextureDescriptor TextureDescriptor;
	TextureDescriptor.Type = CrossRenderer::TextureType::Texture2D;
	TextureDescriptor.Dimensions = glm::uvec2 ( ImageSize.x, ImageSize.y );
	TextureDescriptor.Format = ImageFormat;
	CrossRenderer::TextureHandle Texture = CrossRenderer::CreateTexture ( TextureDescriptor );
	if ( !Texture )
		{
		stbi_image_free ( Image );
		return CrossRenderer::TextureHandle::Invalid;
		}
	if ( CrossRenderer::Load2DTextureData ( Texture, ImageFormat, Image ) == false )
		{
		stbi_image_free ( Image );
		return CrossRenderer::TextureHandle::Invalid;
		}

	stbi_image_free ( Image );
	return Texture;
	}

CrossRenderer::TextureHandle LoadCubemapTexture ( const std::string ImageFile[6], const bool Flip )
	{
	glm::ivec2 ImageSize;
	int Channels;
	CrossRenderer::PixelFormat ImageFormat;
	CrossRenderer::TextureHandle Result;
	CrossRenderer::TextureDescriptor TextureDescriptor;

	stbi_set_flip_vertically_on_load ( Flip ? 1 : 0 );
	stbi_uc *Images[6];
	for ( unsigned Face = 0; Face < 6; ++Face )
		{
		Images[Face] = stbi_load ( ImageFile[Face].c_str (), &ImageSize.x, &ImageSize.y, &Channels, 0 );
		if ( !Images[Face] )
			{
			LOG_ERROR ( "Unable to load texture from '%s'", ImageFile[Face].c_str () );
			goto End;
			}
		switch ( Channels )
			{
			case 3:
				ImageFormat = CrossRenderer::PixelFormat::RedGreenBlue888;
				break;
			case 4:
				ImageFormat = CrossRenderer::PixelFormat::RedGreenBlueAlpha8888;
				break;
			default:
				goto End;
			}
		}
	TextureDescriptor.Type = CrossRenderer::TextureType::TextureCubeMap;
	TextureDescriptor.Dimensions = glm::uvec2 ( ImageSize.x, ImageSize.y );
	TextureDescriptor.Format = ImageFormat;
	Result = CrossRenderer::CreateTexture ( TextureDescriptor );
	if ( !Result )
		goto End;

	void *VoidPointer[6];
	for ( unsigned Face = 0; Face < 6; ++Face )
		VoidPointer[Face] = Images[Face];
	if ( CrossRenderer::LoadCubeMapTextureData ( Result, ImageFormat, VoidPointer ) == false )
		goto End;
End:
	for ( unsigned Face = 0; Face < 6; ++Face )
		stbi_image_free ( Images[Face] );
	return Result;
	}
