#include "TestBase.h"
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void TestBase::OnEvent ( const CrossRenderer::WindowEvent &Event )
    {
    switch ( Event.EventType )
        {
        case CrossRenderer::WindowEventType::WindowResized:
            {
            CrossRenderer::DeleteFramebuffer ( DefaultFramebuffer );
            DefaultFramebuffer = CrossRenderer::CreateFramebuffer ( CrossRenderer::FramebufferDescriptor ( glm::uvec2 ( Event.EventData.WindowResized.Width, Event.EventData.WindowResized.Height ) ) );
            break;
            }
        case CrossRenderer::WindowEventType::KeyPressed:
            {
            if ( Event.EventData.KeyPressed.Key == CrossRenderer::WindowManager::GetKeyCode ( "Escape" ) )
                {
                TestResult = false;
                ShouldQuit = true;
                }
            if ( Event.EventData.KeyPressed.Key == CrossRenderer::WindowManager::GetKeyCode ( "Return" ) )
                {
                TestResult = true;
                ShouldQuit = true;
                }
            break;
            }
        case CrossRenderer::WindowEventType::Quit:
            {
            ShouldQuit = true;
            break;
            }
        default:
            break;
        }
    SpecificOnEvent ( Event );
    }

CrossRenderer::ShaderHandle TestBase::LoadShader ( const std::string &VertexFile, const std::string &GeometryFile, const std::string &FragmentFile )
    {
    std::vector <uint8_t> VShaderFileContents, GShaderFileContents, FShaderFileContents;
    // Load the shader
    std::string Path ( TEST_SOURCE_LOCATION );
    Path.append ( TEST_NAME );
    Path.append ( "/" );
    Path.append ( CrossRenderer::Stringify ( RendererBackend ) );
    Path.append ( "/" );
    if ( LoadFileContents ( Path + VertexFile, VShaderFileContents ) == false )
        {
        LOG_ERROR ( "Unable to load vertex shader from '%s'", VertexFile.c_str() );
        return CrossRenderer::ShaderHandle::invalid;
        }

    if ( ( GeometryFile.length() ) && ( LoadFileContents ( Path + GeometryFile, GShaderFileContents ) == false ) )
        {
        LOG_ERROR ( "Unable to load geometry shader from '%s'", GeometryFile.c_str() );
        return CrossRenderer::ShaderHandle::invalid;
        }
    if ( LoadFileContents ( Path + FragmentFile, FShaderFileContents ) == false )
        {
        LOG_ERROR ( "Unable to load fragment shader from '%s'", FragmentFile.c_str() );
        return CrossRenderer::ShaderHandle::invalid;
        }
    CrossRenderer::ShaderCode ShaderCode;
    ShaderCode.VertexShader = std::string ( VShaderFileContents.begin(), VShaderFileContents.end() );
    if ( GeometryFile.length() )
        ShaderCode.GeometryShader = std::string ( GShaderFileContents.begin(), GShaderFileContents.end() );
    ShaderCode.FragmentShader = std::string ( FShaderFileContents.begin(), FShaderFileContents.end() );
    CrossRenderer::ShaderHandle Shader = CrossRenderer::CreateShader ( ShaderCode );
    if ( !Shader )
        {
        LOG_ERROR ( "Unable to build shader from '%s' and '%s'", VertexFile.c_str(), FragmentFile.c_str() );
        }
    return Shader;
    }

CrossRenderer::TextureHandle TestBase::LoadTexture ( const std::string &ImageFile, const bool Flip )
    {
    glm::ivec2 ImageSize;
    int Channels;
    CrossRenderer::PixelFormat ImageFormat;

    std::string Path ( TEST_SOURCE_LOCATION );
    Path.append ( "Data/" );

    stbi_set_flip_vertically_on_load ( Flip ? 1 : 0 );
    stbi_uc *Image = stbi_load ( ( Path + ImageFile ).c_str(), &ImageSize.x, &ImageSize.y, &Channels, 0 );
    if ( !Image )
        {
        LOG_ERROR ( "Unable to load texture from '%s'", ImageFile.c_str() );
        return CrossRenderer::TextureHandle::invalid;
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
            return CrossRenderer::TextureHandle::invalid;
        }

    CrossRenderer::TextureDescriptor TextureDescriptor;
    TextureDescriptor.Type = CrossRenderer::TextureType::Texture2D;
    TextureDescriptor.Dimensions = glm::uvec2 ( ImageSize.x, ImageSize.y );
    TextureDescriptor.Format = ImageFormat;
    CrossRenderer::TextureHandle Texture = CrossRenderer::CreateTexture ( TextureDescriptor );
    if ( !Texture )
        {
        stbi_image_free ( Image );
        return CrossRenderer::TextureHandle::invalid;
        }
    if ( CrossRenderer::Load2DTextureData ( Texture, ImageFormat, Image ) == false )
        {
        stbi_image_free ( Image );
        return CrossRenderer::TextureHandle::invalid;
        }

    stbi_image_free ( Image );
    return Texture;
    }

TestBase::TestBase ( void )
    {
    }

bool TestBase::Initialize ( const CrossRenderer::RendererBackend NewRendererBackend, const glm::ivec2 &NewWindowPosition, const glm::uvec2 &NewWindowSize )
    {
    CrossRenderer::WindowManager::AddEventListener ( this );
    RendererBackend = NewRendererBackend;

    // CrossRenderer initialization
    CrossRenderer::RendererConfiguration NewConfiguration;
    NewConfiguration.DesiredRendererBackend = NewRendererBackend;
    NewConfiguration.InitialWindowConfigurations.push_back ( CrossRenderer::RenderWindowDescriptor ( NewWindowPosition, NewWindowSize ) );
    NewConfiguration.DesiredWindowBackend = CrossRenderer::WindowManagerBackend::SDL2;
    if ( CrossRenderer::Initialize ( NewConfiguration ) == false )
        return false;

    CrossRenderer::FramebufferDescriptor NewFramebufferDescriptor ( NewWindowSize );
    DefaultFramebuffer = CrossRenderer::CreateFramebuffer ( NewFramebufferDescriptor );

    if ( SpecificInitialize() == false )
        {
        TestResult = false;
        return false;
        }
    return true;
    }

bool TestBase::Shutdown ( void )
    {
    CrossRenderer::DeleteFramebuffer ( DefaultFramebuffer );
    CrossRenderer::Shutdown();
    return true;
    }

bool TestBase::GetResult ( void ) const
    {
    return TestResult;
    }

bool TestBase::RunFrame ( void )
    {
    CrossRenderer::StartRenderToFramebuffer ( DefaultFramebuffer );
    //CrossRenderer::StartFrame ( *CrossRenderer::WindowManager::WindowList.begin() );
    SpecificDraw();
    for ( auto &CommandIterator : RenderCommands )
        {
        CrossRenderer::RunCommand ( CommandIterator );
        }
    CrossRenderer::DisplayFramebuffer ( DefaultFramebuffer, *CrossRenderer::WindowManager::WindowList.begin() );
    CrossRenderer::EndFrame ( *CrossRenderer::WindowManager::WindowList.begin() );
    CrossRenderer::WindowManager::ProcessEvents();
    if ( CrossRenderer::WindowManager::WindowList.size() == 0 )
        ShouldQuit = true;
    return !ShouldQuit;
    }

void TestBase::SpecificDraw ( void )
    {
    }

void TestBase::SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
    {
    }
