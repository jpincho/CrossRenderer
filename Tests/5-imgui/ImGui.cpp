#include "../TestBase.h"
#include <imgui/imgui.h>

class ImGuiTest : public TestBase
    {
    private:
        CrossRenderer::TextureHandle ImGuiFontTexture;
        CrossRenderer::ShaderHandle ImGuiShader;
        CrossRenderer::ShaderAttributeHandle ImGuiPositionShaderAttribute, ImGuiColorShaderAttribute, ImGuiTexCoordShaderAttribute;
        CrossRenderer::ShaderUniformHandle ImGuiMVPMatrixUniform, ImGuiTextureUniform;
        std::vector <CrossRenderer::ShaderBufferHandle> ImGuiBuffers;
        CrossRenderer::RenderCommand TriangleRenderCommand, TexturedQuadRenderCommand;

    public:
        void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
            {
            ImGuiIO &io = ImGui::GetIO();
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
                        return;
                        }
                    case CrossRenderer::WindowEventType::ButtonPressed:
                        {
                        io.MouseDown[Event.EventData.ButtonPressed.Button] = 1;
                        return;
                        }
                    case CrossRenderer::WindowEventType::ButtonReleased:
                        {
                        io.MouseDown[Event.EventData.ButtonReleased.Button] = 0;
                        return;
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
                        return;
                        }
                    case CrossRenderer::WindowEventType::KeyPressed:
                    case CrossRenderer::WindowEventType::KeyReleased:
                        {
                        int key = Event.EventData.KeyPressed.ScanCode;
                        IM_ASSERT ( key >= 0 && key < IM_ARRAYSIZE ( io.KeysDown ) );
                        io.KeysDown[key] = ( Event.EventType == CrossRenderer::WindowEventType::KeyPressed );
                        return;
                        }
                    default:
                        break;
                    }
                }

            switch ( Event.EventType )
                {
                case CrossRenderer::WindowEventType::WindowResized:
                    {
                    io.DisplaySize.x = ( float ) Event.EventData.WindowResized.Width;
                    io.DisplaySize.y = ( float ) Event.EventData.WindowResized.Height;
                    break;
                    }
                case CrossRenderer::WindowEventType::MouseMoved:
                    {
                    io.MousePos.x = ( float ) Event.EventData.MouseMoved.X;
                    io.MousePos.y = ( float ) Event.EventData.MouseMoved.Y;
                    break;
                    }
                default:
                    break;
                }
            }

        bool SpecificInitialize ( void )
            {
            CrossRenderer::ShaderBufferHandle TriangleBuffer, TexturedQuadBuffer;
            CrossRenderer::ShaderHandle TriangleShader, TexturedQuadShader;

            // Create the shapes
            typedef struct
                {
                glm::vec3 Position;
                glm::vec2 TexCoord;
                glm::vec3 Color;
                } Vertex;
            Vertex TriangleVertices[3] =
                {
                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                    {{ 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
                };
            TriangleBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( TriangleVertices, sizeof ( TriangleVertices ) ) );

            Vertex QuadVertices[4] =
                {
                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                    {{ 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
                };
            TexturedQuadBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( QuadVertices, sizeof ( QuadVertices ) ) );

            // Load the shaders
            TriangleShader = LoadShader ( "Triangle.vert", "", "Triangle.frag" );
            if ( !TriangleShader )
                return false;
            TexturedQuadShader = LoadShader ( "TexturedQuad.vert", "", "TexturedQuad.frag" );
            if ( !TexturedQuadShader )
                return false;
            ImGuiShader = LoadShader ( "ImGui.vert", "", "ImGui.frag" );
            if ( !ImGuiShader )
                return false;

            // Load the texture
            CrossRenderer::TextureHandle Texture;
            if ( ! ( Texture = LoadTexture ( std::string ( "WoodCrate/WoodCrate_Diffuse.png" ) ) ) )
                return false;

            // Configure ImGui
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            io.BackendPlatformName = "CrossRenderer";

            io.KeyMap[ImGuiKey_Tab] = CrossRenderer::WindowManager::GetKeyScanCode ( "Tab" );
            io.KeyMap[ImGuiKey_LeftArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Left" );
            io.KeyMap[ImGuiKey_RightArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Right" );;
            io.KeyMap[ImGuiKey_UpArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Up" );
            io.KeyMap[ImGuiKey_DownArrow] = CrossRenderer::WindowManager::GetKeyScanCode ( "Down" );
            io.KeyMap[ImGuiKey_PageUp] = CrossRenderer::WindowManager::GetKeyScanCode ( "PageUp" );
            io.KeyMap[ImGuiKey_PageDown] = CrossRenderer::WindowManager::GetKeyScanCode ( "PgeDown" );
            io.KeyMap[ImGuiKey_Home] = CrossRenderer::WindowManager::GetKeyScanCode ( "Home" );
            io.KeyMap[ImGuiKey_End] = CrossRenderer::WindowManager::GetKeyScanCode ( "End" );
            io.KeyMap[ImGuiKey_Insert] = CrossRenderer::WindowManager::GetKeyScanCode ( "Insert" );
            io.KeyMap[ImGuiKey_Delete] = CrossRenderer::WindowManager::GetKeyScanCode ( "Delete" );
            io.KeyMap[ImGuiKey_Backspace] = CrossRenderer::WindowManager::GetKeyScanCode ( "Backspace" );
            io.KeyMap[ImGuiKey_Space] = CrossRenderer::WindowManager::GetKeyScanCode ( "Space" );
            io.KeyMap[ImGuiKey_Enter] = CrossRenderer::WindowManager::GetKeyScanCode ( "Return" );
            io.KeyMap[ImGuiKey_Escape] = CrossRenderer::WindowManager::GetKeyScanCode ( "Escape" );
            io.KeyMap[ImGuiKey_KeyPadEnter] = CrossRenderer::WindowManager::GetKeyScanCode ( "KpadEnter" );
            io.KeyMap[ImGuiKey_A] = CrossRenderer::WindowManager::GetKeyScanCode ( "a" );
            io.KeyMap[ImGuiKey_C] = CrossRenderer::WindowManager::GetKeyScanCode ( "c" );
            io.KeyMap[ImGuiKey_V] = CrossRenderer::WindowManager::GetKeyScanCode ( "v" );
            io.KeyMap[ImGuiKey_X] = CrossRenderer::WindowManager::GetKeyScanCode ( "x" );
            io.KeyMap[ImGuiKey_Y] = CrossRenderer::WindowManager::GetKeyScanCode ( "y" );
            io.KeyMap[ImGuiKey_Z] = CrossRenderer::WindowManager::GetKeyScanCode ( "z" );

            io.ClipboardUserData = nullptr;

            // Build texture atlas
            unsigned char *FontPixelData;
            glm::ivec2 FontSize;
            int BytesPerPixel;
            io.Fonts->GetTexDataAsRGBA32 ( &FontPixelData, &FontSize.x, &FontSize.y, &BytesPerPixel );
            CrossRenderer::TextureDescriptor FontTextureDescriptor ( FontSize, CrossRenderer::PixelFormat::RedGreenBlueAlpha8888 );

            ImGuiFontTexture = CrossRenderer::Create2DTexture ( FontTextureDescriptor );
            CrossRenderer::Load2DTextureData ( ImGuiFontTexture, CrossRenderer::PixelFormat::RedGreenBlueAlpha8888, FontPixelData, FontSize.x );

            io.Fonts->TexID = ( ImTextureID ) ImGuiFontTexture.key();
            glm::uvec2 WindowSize = CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin() );
            io.DisplaySize.x
                = WindowSize.x;
            io.DisplaySize.y = WindowSize.y;
            glm::ivec2 MousePosition = CrossRenderer::WindowManager::GetMousePosition();
            io.MousePos.x = MousePosition.x;
            io.MousePos.y = MousePosition.y;
            uint32_t MouseButtonStatus = CrossRenderer::WindowManager::GetMouseButtonStatus();
            io.MouseDown[0] = MouseButtonStatus & 1;
            io.MouseDown[1] = MouseButtonStatus & 2;

            // Configure the draw commands
            CrossRenderer::ShaderBufferDataStream TrianglePositionStream, TriangleColorStream;
            CrossRenderer::ShaderAttributeHandle TrianglePositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TriangleShader, "a_Position" );
            CrossRenderer::ShaderUniformHandle TriangleColorShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TriangleShader, "a_Color" );
            CrossRenderer::ShaderUniformHandle TriangleMVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( TriangleShader, "u_MVPMatrix" );

            glm::mat4 MVP;
            MVP = glm::translate ( glm::vec3 ( -0.5f, 0, 0 ) );
            MVP *= glm::scale ( glm::vec3 ( 0.5f, 0.5f, 0.5f ) );

            TrianglePositionStream.BufferHandle = TriangleBuffer;
            TrianglePositionStream.Stride = sizeof ( Vertex );
            TrianglePositionStream.StartOffset = offsetof ( Vertex, Position );
            TrianglePositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TrianglePositionStream.ComponentsPerElement = 3;
            TrianglePositionStream.NormalizeData = false;

            TriangleColorStream.BufferHandle = TriangleBuffer;
            TriangleColorStream.Stride = sizeof ( Vertex );
            TriangleColorStream.StartOffset = offsetof ( Vertex, Color );
            TriangleColorStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TriangleColorStream.ComponentsPerElement = 3;
            TriangleColorStream.NormalizeData = false;

            TriangleRenderCommand.Shader = TriangleShader;
            TriangleRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TrianglePositionShaderAttribute,
                        TrianglePositionStream ) );
            TriangleRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TriangleColorShaderAttribute,
                        TriangleColorStream ) );
            TriangleRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( TriangleMVPMatrixUniform, MVP ) );
            TriangleRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
            TriangleRenderCommand.StartVertex = 0;
            TriangleRenderCommand.VertexCount = 3;
            RenderCommands.push_back ( TriangleRenderCommand );

            CrossRenderer::ShaderBufferDataStream TexturedQuadPositionStream, TexturedQuadTextureStream;
            CrossRenderer::ShaderAttributeHandle TexturedQuadPositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TexturedQuadShader, "a_Position" );
            CrossRenderer::ShaderAttributeHandle TexturedQuadTextureCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TexturedQuadShader, "a_TexCoord" );
            CrossRenderer::ShaderUniformHandle TexturedQuadTextureUniform = CrossRenderer::GetShaderUniformHandle ( TexturedQuadShader, "u_Texture" );
            CrossRenderer::ShaderUniformHandle TexturedQuadMVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( TexturedQuadShader, "u_MVPMatrix" );

            MVP = glm::translate ( glm::vec3 ( 0.5f, 0, 0 ) );
            MVP *= glm::scale ( glm::vec3 ( 0.5f, 0.5f, 0.5f ) );

            TexturedQuadPositionStream.BufferHandle = TexturedQuadBuffer;
            TexturedQuadPositionStream.Stride = sizeof ( Vertex );
            TexturedQuadPositionStream.StartOffset = offsetof ( Vertex, Position );
            TexturedQuadPositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TexturedQuadPositionStream.ComponentsPerElement = 3;
            TexturedQuadPositionStream.NormalizeData = false;

            TexturedQuadTextureStream.BufferHandle = TexturedQuadBuffer;
            TexturedQuadTextureStream.Stride = sizeof ( Vertex );
            TexturedQuadTextureStream.StartOffset = offsetof ( Vertex, TexCoord );
            TexturedQuadTextureStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TexturedQuadTextureStream.ComponentsPerElement = 2;
            TexturedQuadTextureStream.NormalizeData = false;

            TexturedQuadRenderCommand.Shader = TexturedQuadShader;
            TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TexturedQuadPositionShaderAttribute,
                        TexturedQuadPositionStream ) );
            TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TexturedQuadTextureCoordShaderAttribute,
                        TexturedQuadTextureStream ) );
            TexturedQuadRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TexturedQuadTextureUniform, Texture ) );
            TexturedQuadRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( TexturedQuadMVPMatrixUniform, MVP ) );
            TexturedQuadRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
            TexturedQuadRenderCommand.StartVertex = 0;
            TexturedQuadRenderCommand.VertexCount = 4;
            RenderCommands.push_back ( TexturedQuadRenderCommand );

            ImGuiPositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiShader, "a_Position" );
            ImGuiTexCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiShader, "a_TexCoord" );
            ImGuiColorShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiShader, "a_Color" );

            ImGuiMVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( ImGuiShader, "u_MVPMatrix" );
            ImGuiTextureUniform = CrossRenderer::GetShaderUniformHandle ( ImGuiShader, "u_Texture" );

            return true;
            }

        void SpecificDraw ( void )
            {
            RenderCommands.clear();
            RenderCommands.push_back ( TriangleRenderCommand );
            RenderCommands.push_back ( TexturedQuadRenderCommand );
            for ( auto Iterator : ImGuiBuffers )
                CrossRenderer::DeleteShaderBuffer ( Iterator );
            ImGuiBuffers.clear();

            ImGuiIO &io = ImGui::GetIO();
            glm::ivec2 MousePos = CrossRenderer::WindowManager::GetMousePosition();
            uint32_t MouseButtons = CrossRenderer::WindowManager::GetMouseButtonStatus();
            io.MousePos.x = MousePos.x;
            io.MousePos.y = MousePos.y;
            io.MouseDown[0] = MouseButtons & 1;
            io.MouseDown[1] = MouseButtons & 2;

            io.DeltaTime = 1.0f / 60.0f;

            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();
            ImGui::EndFrame();

            ImDrawData *draw_data = ImGui::GetDrawData();
            int fb_width = ( int ) ( draw_data->DisplaySize.x * draw_data->FramebufferScale.x );
            int fb_height = ( int ) ( draw_data->DisplaySize.y * draw_data->FramebufferScale.y );
            if ( fb_width <= 0 || fb_height <= 0 )
                return;

            for ( int n = 0; n < draw_data->CmdListsCount; n++ )
                {
                const ImDrawList *cmd_list = draw_data->CmdLists[n];
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
                ImGuiBuffers.push_back ( DataBufferHandle );
                ImGuiBuffers.push_back ( IndexBufferHandle );
                for ( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++ )
                    {
                    const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
                    CrossRenderer::RenderCommand Command;

                    Command.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( ImGuiTextureUniform,
                                                        CrossRenderer::TextureBindSettings ( CrossRenderer::TextureHandle ( ( intptr_t ) pcmd->TextureId ) ) ) );

                    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
                    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
                    ImVec4 clip_rect;
                    clip_rect.x = ( pcmd->ClipRect.x - clip_off.x ) * clip_scale.x;
                    clip_rect.y = ( pcmd->ClipRect.y - clip_off.y ) * clip_scale.y;
                    clip_rect.z = ( pcmd->ClipRect.z - clip_off.x ) * clip_scale.x;
                    clip_rect.w = ( pcmd->ClipRect.w - clip_off.y ) * clip_scale.y;
                    Command.State.Scissor.Set ( glm::uvec2 ( clip_rect.x, fb_height - clip_rect.w ), glm::uvec2 ( clip_rect.z - clip_rect.x, clip_rect.w - clip_rect.y ) );

                    Command.Shader = ImGuiShader;
                    Command.Primitive = CrossRenderer::PrimitiveType::TriangleList;
                    Command.IndexBuffer = IndexBufferHandle;
                    Command.IndexBufferStream = IndexBufferStream;
                    Command.StartVertex = IndexOffset;
                    Command.VertexCount = pcmd->ElemCount;
                    Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiPositionShaderAttribute, VertexStream ) );
                    Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiTexCoordShaderAttribute, TexCoordStream ) );
                    Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiColorShaderAttribute, ColorStream ) );

                    Command.State.Blending.Set ( CrossRenderer::BlendMode::SourceAlpha, CrossRenderer::BlendMode::OneMinusSourceAlpha );

                    float L = draw_data->DisplayPos.x;
                    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
                    float T = draw_data->DisplayPos.y;
                    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

                    Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( ImGuiMVPMatrixUniform, glm::ortho ( L, R, B, T ) ) );
                    assert ( CrossRenderer::SanityCheckRenderCommand ( Command ) );
                    RenderCommands.push_back ( Command );
                    IndexOffset += pcmd->ElemCount;
                    }
                }
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        ImGuiTest Test;
        if ( Test.Initialize ( RenderersToTest[index], glm::ivec2 ( 0, 0 ), glm::uvec2 ( 800, 600 ) ) == false )
            return -1;
        bool ShouldQuit = false;
        while ( !ShouldQuit )
            {
            if ( Test.RunFrame() == false )
                ShouldQuit = true;
            }
        Test.Shutdown();
        }
    return 0;
    }
