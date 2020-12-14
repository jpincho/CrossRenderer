#include "TestBase.h"
#include <algorithm>

void TestBase::OnEvent ( const CrossRenderer::WindowEvent &Event )
    {
    switch ( Event.EventType )
        {
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
    if ( LoadFileContents ( VertexFile, VShaderFileContents ) == false )
        {
        LOG_ERROR ( "Unable to load vertex shader from '%s'", VertexFile.c_str() );
        return CrossRenderer::ShaderHandle::invalid;
        }

    if ( ( GeometryFile.length() ) && ( LoadFileContents ( GeometryFile, GShaderFileContents ) == false ) )
        {
        LOG_ERROR ( "Unable to load geometry shader from '%s'", GeometryFile.c_str() );
        return CrossRenderer::ShaderHandle::invalid;
        }
    if ( LoadFileContents ( FragmentFile, FShaderFileContents ) == false )
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

    std::string Path ( TEST_SOURCE_LOCATION );
    Path.append ( "/Data/ImGui Shader/" );
    Path.append ( CrossRenderer::Stringify ( RendererBackend ) );

    if ( SpecificInitialize() == false )
        {
        TestResult = false;
        return false;
        }
    return true;
    }

bool TestBase::Shutdown ( void )
    {
    CrossRenderer::Shutdown();
    return true;
    }

bool TestBase::GetResult ( void ) const
    {
    return TestResult;
    }

bool TestBase::RunFrame ( void )
    {
    CrossRenderer::StartFrame ( *CrossRenderer::WindowManager::WindowList.begin() );
    SpecificDraw();
    for ( auto &CommandIterator : RenderCommands )
        {
        CrossRenderer::RunCommand ( CommandIterator );
        }
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
