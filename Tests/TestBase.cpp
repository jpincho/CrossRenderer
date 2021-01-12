#include "TestBase.h"
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void TestBase::OnEvent ( const CrossRenderer::WindowEvent &Event )
    {
    ImGuiIO& io = ImGui::GetIO();
    if ( ImGuiProcessEvent ( Event ) == true )
        return;

    switch ( Event.EventType )
        {
        case CrossRenderer::WindowEventType::MouseMoved:
            {
            io.MousePos.x = ( float ) Event.EventData.MouseMoved.X;
            io.MousePos.y = ( float ) Event.EventData.MouseMoved.Y;
            break;
            }
        case CrossRenderer::WindowEventType::WindowResized:
            {
            CrossRenderer::DeleteFramebuffer ( DefaultFramebuffer );
            DefaultFramebuffer = CrossRenderer::CreateFramebuffer ( CrossRenderer::FramebufferDescriptor ( glm::uvec2 ( Event.EventData.WindowResized.Width, Event.EventData.WindowResized.Height ) ) );
            io.DisplaySize.x = ( float ) Event.EventData.WindowResized.Width;
            io.DisplaySize.y = ( float ) Event.EventData.WindowResized.Height;
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

CrossRenderer::TextureHandle TestBase::LoadCubemapTexture ( const std::string ImageFile[6], const bool Flip )
    {
    glm::ivec2 ImageSize;
    int Channels;
    CrossRenderer::PixelFormat ImageFormat;
    CrossRenderer::TextureHandle Result;
    CrossRenderer::TextureDescriptor TextureDescriptor;

    std::string Path ( TEST_SOURCE_LOCATION );
    Path.append ( "Data/" );

    stbi_set_flip_vertically_on_load ( Flip ? 1 : 0 );
    stbi_uc *Images[6];
    for ( unsigned Face = 0; Face < 6; ++Face )
        {
        Images[Face] = stbi_load ( ( Path + ImageFile[Face] ).c_str(), &ImageSize.x, &ImageSize.y, &Channels, 0 );
        if ( !Images[Face] )
            {
            LOG_ERROR ( "Unable to load texture from '%s'", ImageFile[Face].c_str() );
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

bool TestBase::LoadModel ( const std::string Path, const std::string Filename, std::vector <Material> &Materials, std::vector <MeshRenderData> &MeshRenderDataArray )
    {
    ModelLoader LoadedModel;
    if ( LoadedModel.Set ( std::string ( TEST_SOURCE_LOCATION ) + std::string ( "/Data/" ) + Path, Filename ) == false )
        return false;
    if ( LoadedModel.Load() == false )
        return false;

    for ( auto &MaterialIterator : LoadedModel.Materials )
        {
        Material NewMaterial;
        NewMaterial.Name = MaterialIterator.Name;
        NewMaterial.Diffuse = MaterialIterator.Diffuse;
        NewMaterial.Specular = MaterialIterator.Specular;
        NewMaterial.Emissive = MaterialIterator.Emissive;
        NewMaterial.Shininess = MaterialIterator.Shininess;
        for ( auto TextureTypeIterator = 0; TextureTypeIterator < ( int ) Material::MaterialTextureType::COUNT; ++TextureTypeIterator )
            {
            CrossRenderer::TextureBindSettings Settings;
            if ( MaterialIterator.Textures[TextureTypeIterator].empty() == true )
                continue;
            if ( ! ( Settings.Handle = LoadTexture ( Path + MaterialIterator.Textures[TextureTypeIterator] ) ) )
                return false;
            Settings.WrapSettings = MaterialIterator.WrapSettings[TextureTypeIterator];
            NewMaterial.Textures[TextureTypeIterator] = Settings;
            }
        Materials.push_back ( NewMaterial );
        }

    for ( auto &ModelIterator : LoadedModel.Models )
        {
        for ( auto &MeshIterator : ModelIterator.Meshes )
            {
            MeshRenderData NewData;
            NewData.Index = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.Indices.data(), MeshIterator.Indices.size() * sizeof ( uint32_t ) ) );
            NewData.Vertex = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.Vertices.data(), MeshIterator.Vertices.size() * sizeof ( glm::vec3 ) ) );
            NewData.Normal = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.Normals.data(), MeshIterator.Normals.size() * sizeof ( glm::vec3 ) ) );
            NewData.TexCoord = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.TexCoords.data(), MeshIterator.TexCoords.size() * sizeof ( glm::uvec2 ) ) );
            NewData.VertexCount = MeshIterator.Indices.size();

            for ( auto &MaterialIterator : Materials )
                {
                if ( MeshIterator.MaterialName == MaterialIterator.Name )
                    {
                    NewData.MeshMaterial = MaterialIterator;
                    break;
                    }
                }
            MeshRenderDataArray.push_back ( NewData );
            }
        }
    return true;
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
    if ( InitializeImGui() == false )
        return false;

    CrossRenderer::FramebufferDescriptor NewFramebufferDescriptor ( NewWindowSize );
    DefaultFramebuffer = CrossRenderer::CreateFramebuffer ( NewFramebufferDescriptor );
    UseOwnFramebuffer = true;
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
    bool OwnFramebuffer = UseOwnFramebuffer;
    if ( OwnFramebuffer )
        CrossRenderer::StartRenderToFramebuffer ( DefaultFramebuffer );
    else
        CrossRenderer::StartFrame ( *CrossRenderer::WindowManager::WindowList.begin() );
    SpecificDraw();
    for ( auto &CommandIterator : RenderCommands )
        {
        CrossRenderer::RunCommand ( CommandIterator );
        }
    RenderImGui();
    if ( OwnFramebuffer )
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

void TestBase::SpecificOnEvent ( const CrossRenderer::WindowEvent & )
    {
    }

void TestBase::SpecificImGuiDraw ( void )
    {

    }

bool TestBase::InitializeImGui ( void )
    {
    std::vector <uint8_t> VShaderFileContents, FShaderFileContents;
    // Load the shader
    std::string Path ( TEST_SOURCE_LOCATION );
    Path.append ( "Data/ImGui/" );
    Path.append ( CrossRenderer::Stringify ( RendererBackend ) );
    Path.append ( "/" );
    if ( LoadFileContents ( Path + std::string ( "ImGui.vert" ), VShaderFileContents ) == false )
        {
        LOG_ERROR ( "Unable to load ImGui vertex shader" );
        return false;
        }
    if ( LoadFileContents ( Path + std::string ( "ImGui.frag" ), FShaderFileContents ) == false )
        {
        LOG_ERROR ( "Unable to load ImGui fragment shader" );
        return false;
        }
    CrossRenderer::ShaderCode ShaderCode;
    ShaderCode.VertexShader = std::string ( VShaderFileContents.begin(), VShaderFileContents.end() );
    ShaderCode.FragmentShader = std::string ( FShaderFileContents.begin(), FShaderFileContents.end() );
    ImGuiShader = CrossRenderer::CreateShader ( ShaderCode );
    if ( !ImGuiShader )
        {
        LOG_ERROR ( "Unable to build ImGui shader" );
        return false;
        }

    ImGuiPositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiShader, "a_Position" );
    ImGuiTexCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiShader, "a_TexCoord" );
    ImGuiColorShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiShader, "a_Color" );

    ImGuiMVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( ImGuiShader, "u_MVPMatrix" );
    ImGuiTextureUniform = CrossRenderer::GetShaderUniformHandle ( ImGuiShader, "u_Texture" );

    // Configure ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "CrossRenderer";

    io.KeyMap[ImGuiKey_Tab] = CrossRenderer::WindowManager::GetKeyScanCode ( "Tab" );
    io.KeyMap[ImGuiKey_LeftArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Left" );
    io.KeyMap[ImGuiKey_RightArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Right" );;
    io.KeyMap[ImGuiKey_UpArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Up" );
    io.KeyMap[ImGuiKey_DownArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Down" );
    io.KeyMap[ImGuiKey_PageUp] = CrossRenderer::WindowManager::GetKeyScanCode ( "PageUp" );
    io.KeyMap[ImGuiKey_PageDown] = CrossRenderer::WindowManager::GetKeyScanCode ( "PageDown" );
    io.KeyMap[ImGuiKey_Home] = CrossRenderer::WindowManager::GetKeyScanCode ( "Home" );
    io.KeyMap[ImGuiKey_End] = CrossRenderer::WindowManager::GetKeyScanCode ( "End" );
    io.KeyMap[ImGuiKey_Insert] = CrossRenderer::WindowManager::GetKeyScanCode ( "Insert" );
    io.KeyMap[ImGuiKey_Delete] = CrossRenderer::WindowManager::GetKeyScanCode ( "Delete" );
    io.KeyMap[ImGuiKey_Backspace] = CrossRenderer::WindowManager::GetKeyScanCode ( "Backspace" );
    io.KeyMap[ImGuiKey_Space] = CrossRenderer::WindowManager::GetKeyScanCode ( "Space" );
    io.KeyMap[ImGuiKey_Enter] = CrossRenderer::WindowManager::GetKeyScanCode ( "Return" );
    io.KeyMap[ImGuiKey_Escape] = CrossRenderer::WindowManager::GetKeyScanCode ( "Escape" );
    io.KeyMap[ImGuiKey_KeyPadEnter] = CrossRenderer::WindowManager::GetKeyScanCode ( "Keypad Enter" );
    io.KeyMap[ImGuiKey_A] = CrossRenderer::WindowManager::GetKeyScanCode ( "a" );
    io.KeyMap[ImGuiKey_C] = CrossRenderer::WindowManager::GetKeyScanCode ( "c" );
    io.KeyMap[ImGuiKey_V] = CrossRenderer::WindowManager::GetKeyScanCode ( "v" );
    io.KeyMap[ImGuiKey_X] = CrossRenderer::WindowManager::GetKeyScanCode ( "x" );
    io.KeyMap[ImGuiKey_Y] = CrossRenderer::WindowManager::GetKeyScanCode ( "y" );
    io.KeyMap[ImGuiKey_Z] = CrossRenderer::WindowManager::GetKeyScanCode ( "z" );

    io.ClipboardUserData = nullptr;

    // Build texture atlas
    unsigned char* FontPixelData;
    glm::ivec2 FontSize;
    int BytesPerPixel;
    io.Fonts->GetTexDataAsRGBA32 ( &FontPixelData, &FontSize.x, &FontSize.y, &BytesPerPixel );
    CrossRenderer::TextureDescriptor FontTextureDescriptor ( FontSize, CrossRenderer::PixelFormat::RedGreenBlueAlpha8888 );

    ImGuiFontTexture = CrossRenderer::CreateTexture ( FontTextureDescriptor );
    CrossRenderer::Load2DTextureData ( ImGuiFontTexture, CrossRenderer::PixelFormat::RedGreenBlueAlpha8888, FontPixelData );

    io.Fonts->TexID = ( ImTextureID ) ImGuiFontTexture.key();
    glm::uvec2 WindowSize = CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin() );
    io.DisplaySize.x = ( float ) WindowSize.x;
    io.DisplaySize.y = ( float ) WindowSize.y;
    glm::ivec2 MousePosition = CrossRenderer::WindowManager::GetMousePosition();
    io.MousePos.x = ( float ) MousePosition.x;
    io.MousePos.y = ( float ) MousePosition.y;
    uint32_t MouseButtonStatus = CrossRenderer::WindowManager::GetMouseButtonStatus();
    io.MouseDown[0] = MouseButtonStatus & 1;
    io.MouseDown[1] = MouseButtonStatus & 2;
    return true;
    }

bool TestBase::ImGuiProcessEvent ( const CrossRenderer::WindowEvent& Event )
    {
    ImGuiIO& io = ImGui::GetIO();
    if ( io.WantCaptureMouse )
        {
        switch ( Event.EventType )
            {
            case CrossRenderer::WindowEventType::MouseWheel:
                {
                if ( Event.EventData.MouseWheel.X > 0 ) io.MouseWheelH += 1;
                if ( Event.EventData.MouseWheel.X < 0 ) io.MouseWheelH -= 1;
                if ( Event.EventData.MouseWheel.Y > 0 ) io.MouseWheel += 1;
                if ( Event.EventData.MouseWheel.Y < 0 ) io.MouseWheel -= 1;
                return true;
                }
            case CrossRenderer::WindowEventType::ButtonPressed:
                {
                io.MouseDown[Event.EventData.ButtonPressed.Button] = 1;
                return true;
                }
            case CrossRenderer::WindowEventType::ButtonReleased:
                {
                io.MouseDown[Event.EventData.ButtonReleased.Button] = 0;
                return true;
                }
            default:
                break;
            }
        }
    if ( io.WantCaptureKeyboard )
        {
        switch ( Event.EventType )
            {
            case CrossRenderer::WindowEventType::TextInput:
                {
                io.AddInputCharactersUTF8 ( Event.EventData.TextInput.Text );
                return true;
                }
            case CrossRenderer::WindowEventType::KeyPressed:
            case CrossRenderer::WindowEventType::KeyReleased:
                {
                int key = Event.EventData.KeyPressed.ScanCode;
                IM_ASSERT ( key >= 0 && key < IM_ARRAYSIZE ( io.KeysDown ) );
                io.KeysDown[key] = ( Event.EventType == CrossRenderer::WindowEventType::KeyPressed );
                return true;
                }
            default:
                break;
            }
        }
    return false;
    }

void TestBase::RenderImGui ( void )
    {
    //for ( auto Iterator : ImGuiBuffers )
    //CrossRenderer::DeleteShaderBuffer ( Iterator );
    //ImGuiBuffers.clear();

    ImGuiIO& io = ImGui::GetIO();
    glm::ivec2 MousePos = CrossRenderer::WindowManager::GetMousePosition();
    uint32_t MouseButtons = CrossRenderer::WindowManager::GetMouseButtonStatus();
    io.MousePos.x = ( float ) MousePos.x;
    io.MousePos.y = ( float ) MousePos.y;
    io.MouseDown[0] = MouseButtons & 1;
    io.MouseDown[1] = MouseButtons & 2;

    io.DeltaTime = 1.0f / 60.0f;

    ImGui::NewFrame();
    SpecificImGuiDraw();
    ImGui::EndFrame();
    ImGui::Render();

    ImDrawData* draw_data = ImGui::GetDrawData();
    int fb_width = ( int ) ( draw_data->DisplaySize.x * draw_data->FramebufferScale.x );
    int fb_height = ( int ) ( draw_data->DisplaySize.y * draw_data->FramebufferScale.y );
    if ( fb_width <= 0 || fb_height <= 0 )
        return;

    for ( int n = 0; n < draw_data->CmdListsCount; n++ )
        {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        int IndexOffset = 0;

        // Upload both buffers to GPU
        CrossRenderer::ShaderBufferHandle DataBufferHandle = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size() * sizeof ( ImDrawVert ), CrossRenderer::ShaderBufferAccessType::Stream ) );
        CrossRenderer::ShaderBufferHandle IndexBufferHandle = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size() * sizeof ( ImDrawIdx ), CrossRenderer::ShaderBufferAccessType::Stream ) );

        // Create all attribute streams
        CrossRenderer::ShaderBufferDataStream IndexBufferStream ( IndexBufferHandle,
                0,
                0,
                sizeof ( ImDrawIdx ) == 2 ? CrossRenderer::ShaderBufferComponentType::Unsigned16 : CrossRenderer::ShaderBufferComponentType::Unsigned32,
                1 );
        CrossRenderer::ShaderBufferDataStream VertexStream ( DataBufferHandle,
                offsetof ( ImDrawVert, pos ),
                sizeof ( ImDrawVert ),
                CrossRenderer::ShaderBufferComponentType::Float,
                2 );
        CrossRenderer::ShaderBufferDataStream TexCoordStream ( DataBufferHandle,
                offsetof ( ImDrawVert, uv ),
                sizeof ( ImDrawVert ),
                CrossRenderer::ShaderBufferComponentType::Float,
                2 );
        CrossRenderer::ShaderBufferDataStream ColorStream ( DataBufferHandle,
                offsetof ( ImDrawVert, col ),
                sizeof ( ImDrawVert ),
                CrossRenderer::ShaderBufferComponentType::Unsigned8,
                4,
                true );
        //ImGuiBuffers.push_back ( DataBufferHandle );
        //ImGuiBuffers.push_back ( IndexBufferHandle );
        CrossRenderer::RenderCommand Command;
        Command.Shader = ImGuiShader;
        Command.Primitive = CrossRenderer::PrimitiveType::TriangleList;
        Command.IndexBuffer = IndexBufferHandle;
        Command.IndexBufferStream = IndexBufferStream;
        Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiPositionShaderAttribute, VertexStream ) );
        Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiTexCoordShaderAttribute, TexCoordStream ) );
        Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiColorShaderAttribute, ColorStream ) );

        Command.State.Blending.Set ( CrossRenderer::BlendMode::SourceAlpha, CrossRenderer::BlendMode::OneMinusSourceAlpha );

        for ( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++ )
            {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

            ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
            ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
            ImVec4 clip_rect;
            clip_rect.x = ( pcmd->ClipRect.x - clip_off.x ) * clip_scale.x;
            clip_rect.y = ( pcmd->ClipRect.y - clip_off.y ) * clip_scale.y;
            clip_rect.z = ( pcmd->ClipRect.z - clip_off.x ) * clip_scale.x;
            clip_rect.w = ( pcmd->ClipRect.w - clip_off.y ) * clip_scale.y;

            float L = draw_data->DisplayPos.x;
            float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
            float T = draw_data->DisplayPos.y;
            float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

            Command.TextureBindings.clear();
            Command.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( ImGuiTextureUniform,
                                                CrossRenderer::TextureBindSettings ( CrossRenderer::TextureHandle ( ( intptr_t ) pcmd->TextureId ) ) ) );
            Command.State.Scissor.Set ( glm::uvec2 ( clip_rect.x, fb_height - clip_rect.w ), glm::uvec2 ( clip_rect.z - clip_rect.x, clip_rect.w - clip_rect.y ) );
            Command.StartVertex = IndexOffset;
            Command.VertexCount = pcmd->ElemCount;
            Command.UniformValues.clear();
            Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( ImGuiMVPMatrixUniform, glm::ortho ( L, R, B, T ) ) );
            assert ( CrossRenderer::SanityCheckRenderCommand ( Command ) );
            CrossRenderer::RunCommand ( Command );
            IndexOffset += pcmd->ElemCount;
            }
        CrossRenderer::DeleteShaderBuffer ( DataBufferHandle );
        CrossRenderer::DeleteShaderBuffer ( IndexBufferHandle );
        }
    }
