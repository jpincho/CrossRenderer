#include "Test-ImGui.hpp"
#include <imgui/imgui.h>
#include "../Internal/Logger.hpp"

bool ImGuiTest::SpecificInitialize ( void )
	{
	struct Vertex
		{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		};

	Vertex Vertices[24] =
		{
			{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}},

			{{ 1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},

			{{ 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
			{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
			{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},

			{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
			{{-1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}},

			{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
			{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},

			{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}},
		};
	uint16_t Indices[] =
		{
		0, 1, 2,
		1, 3, 2,

		0 + 4, 1 + 4, 2 + 4,
		1 + 4, 3 + 4, 2 + 4,

		0 + 8, 1 + 8, 2 + 8,
		1 + 8, 3 + 8, 2 + 8,

		0 + 12, 1 + 12, 2 + 12,
		1 + 12, 3 + 12, 2 + 12,

		0 + 16, 1 + 16, 2 + 16,
		1 + 16, 3 + 16, 2 + 16,

		0 + 20, 1 + 20, 2 + 20,
		1 + 20, 3 + 20, 2 + 20,
		};

	glm::uvec2 WindowSize = CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () );
	CubeData.ProjectionMatrix = glm::perspective ( glm::half_pi<float> (), (float) WindowSize.x / (float) WindowSize.y, 0.1f, 10.0f );
	CubeData.ModelMatrix = glm::identity<glm::mat4> ();
	CubeData.ViewMatrix = glm::lookAt ( glm::vec3 ( 0.0f, 0.0f, -2.0f ), glm::zero<glm::vec3> (), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );

	CrossRenderer::ShaderBufferDescriptor DataDescriptor ( Vertices, sizeof ( Vertices ) );
	CubeData.DataBufferHandle = CrossRenderer::CreateShaderBuffer ( DataDescriptor );
	CrossRenderer::ShaderBufferDescriptor IndexDescriptor ( Indices, sizeof ( Indices ) );
	CubeData.IndexBufferHandle = CrossRenderer::CreateShaderBuffer ( IndexDescriptor );

	CubeData.ShaderHandle = LoadShader (
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/SpinningCubeTest.vert" ), "",
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/SpinningCubeTest.frag" ) );
	if ( !CubeData.ShaderHandle )
		return false;

	CrossRenderer::SetFramebufferClearColor ( Framebuffer, glm::vec4 ( 0.5f, 0.5f, 0.5f, 1.0f ) );
	CubeData.TextureHandle = LoadTexture ( std::string ( TESTBED_DATA_PATH ) + std::string ( "/batman.jpg" ) );
	if ( !CubeData.TextureHandle )
		return false;

	CrossRenderer::ShaderBufferDataStream PositionStream ( CubeData.DataBufferHandle,
														  offsetof ( Vertex, Position ),
														  sizeof ( Vertex ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  3 );
	CrossRenderer::ShaderBufferDataStream TexCoordStream ( CubeData.DataBufferHandle,
														  offsetof ( Vertex, TexCoord ),
														  sizeof ( Vertex ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  2 );
	CrossRenderer::ShaderBufferDataStream IndexStream ( CubeData.IndexBufferHandle,
														0,
														sizeof ( uint16_t ),
														CrossRenderer::ShaderBufferComponentType::Unsigned16,
														1 );
	CrossRenderer::ShaderAttributeHandle PositionAttributeHandle, TexCoordAttributeHandle;
	CrossRenderer::ShaderUniformHandle TextureUniformHandle;
	GET_ATTRIBUTE ( PositionAttributeHandle, CubeData.ShaderHandle, "a_VertexPosition" );
	GET_ATTRIBUTE ( TexCoordAttributeHandle, CubeData.ShaderHandle, "a_TexCoord" );
	GET_UNIFORM ( TextureUniformHandle, CubeData.ShaderHandle, "u_Texture" );
	GET_UNIFORM ( CubeData.MVPUniformHandle, CubeData.ShaderHandle, "u_MVP" );
	CrossRenderer::TextureBindSettings TextureBindSettings ( CubeData.TextureHandle );

	CubeData.RenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
	CubeData.RenderCommand.Shader = CubeData.ShaderHandle;
	CubeData.RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( PositionAttributeHandle, PositionStream ) );
	CubeData.RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( TexCoordAttributeHandle, TexCoordStream ) );
	CubeData.RenderCommand.StartVertex = 0;
	CubeData.RenderCommand.VertexCount = 36;
	CubeData.RenderCommand.InstanceCount = 1;
	CubeData.RenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniformHandle, TextureBindSettings ) );
	CubeData.RenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CubeData.MVPUniformHandle, CubeData.ProjectionMatrix * CubeData.ViewMatrix * CubeData.ModelMatrix ) );
	CubeData.RenderCommand.IndexBufferStream = IndexStream;
	CubeData.RenderCommand.State.DepthTest.Set ( CrossRenderer::DepthTestMode::Less );
	assert ( CrossRenderer::SanityCheckRenderCommand ( CubeData.RenderCommand ) );
	TotalTime = 0.0f;
	return InitializeImGui ();
	}

bool ImGuiTest::SpecificShutdown ( void )
	{
	if ( CubeData.DataBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( CubeData.DataBufferHandle );
		CubeData.DataBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	if ( CubeData.IndexBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( CubeData.IndexBufferHandle );
		CubeData.IndexBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	if ( CubeData.ShaderHandle )
		{
		CrossRenderer::DeleteShader ( CubeData.ShaderHandle );
		CubeData.ShaderHandle = CrossRenderer::ShaderHandle::Invalid;
		}
	if ( CubeData.TextureHandle )
		{
		CrossRenderer::DeleteTexture ( CubeData.TextureHandle );
		CubeData.TextureHandle = CrossRenderer::TextureHandle::Invalid;
		}

	if ( ImGuiData.ShaderHandle )
		{
		CrossRenderer::DeleteShader ( ImGuiData.ShaderHandle );
		ImGuiData.ShaderHandle = CrossRenderer::ShaderHandle::Invalid;
		}
	if ( ImGuiData.FontTexture )
		{
		CrossRenderer::DeleteTexture ( ImGuiData.FontTexture );
		ImGuiData.FontTexture = CrossRenderer::TextureHandle::Invalid;
		}
	if ( ImGuiData.DataBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( ImGuiData.DataBufferHandle );
		ImGuiData.DataBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	if ( ImGuiData.IndexBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( ImGuiData.IndexBufferHandle );
		ImGuiData.IndexBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	return true;
	}

bool ImGuiTest::SpecificFrame ( const float TimeDelta )
	{
	ImGuiIO &io = ImGui::GetIO ();
	io.DeltaTime = TimeDelta;
	TotalTime += TimeDelta;

	ImGui::NewFrame ();

	ImGui::ShowDemoWindow ();

	ImGui::Begin ( "Status" );
	CrossRenderer::RenderWindowHandle WindowHandle = *CrossRenderer::WindowManager::WindowList.begin ();
	ImGui::LabelText ( "Time delta", "%f", TimeDelta );
	ImGui::LabelText ( "Total time", "%f", TotalTime );
	ImGui::LabelText ( "Window size", "%ux%u", CrossRenderer::WindowManager::GetWindowSize ( WindowHandle ).x, CrossRenderer::WindowManager::GetWindowSize ( WindowHandle ).y );
	ImGui::LabelText ( "Window pos", "%dx%d", CrossRenderer::WindowManager::GetWindowPosition ( WindowHandle ).x, CrossRenderer::WindowManager::GetWindowPosition ( WindowHandle ).y );
	bool Fullscreen = ( CrossRenderer::WindowManager::GetWindowState ( WindowHandle ) == CrossRenderer::WindowManager::WindowState::Maximized );
	if ( ImGui::Checkbox ( "Fullscreen", &Fullscreen ) )
		{
		CrossRenderer::WindowManager::SetWindowState ( WindowHandle, Fullscreen ? CrossRenderer::WindowManager::WindowState::Maximized : CrossRenderer::WindowManager::WindowState::Windowed );
		}
	ImGui::Checkbox ( "Deferred", &ImGuiData.DeferredRendering );
	ImGui::LabelText ( "Mouse pos", "%ux%u", CrossRenderer::WindowManager::GetMousePosition ().x, CrossRenderer::WindowManager::GetMousePosition ().y );

	ImGui::End ();
	EndImGuiFrame ();
	CubeData.ModelMatrix = glm::rotate ( glm::quarter_pi<float> () * TimeDelta, glm::vec3 ( 0.0f, 1.0f, 0.0f ) ) * CubeData.ModelMatrix;
	CubeData.RenderCommand.UniformValues[0].UniformValue = CubeData.ProjectionMatrix * CubeData.ViewMatrix * CubeData.ModelMatrix;
	CrossRenderer::RunCommand ( CubeData.RenderCommand );
	for ( const auto &RenderCommandIterator : ImGuiData.RenderCommands )
		CrossRenderer::RunCommand ( RenderCommandIterator );
	return true;
	}

void ImGuiTest::SpecificOnEvent ( const CrossRenderer::WindowManager::WindowEvent &Event )
	{
	ImGuiIO &io = ImGui::GetIO ();

	if ( io.WantCaptureKeyboard )
		{
		switch ( Event.EventType )
			{
			case CrossRenderer::WindowManager::WindowEventType::KeyPressed:
				{
				io.KeysDown[(uint32_t) Event.EventData.KeyPressed.Key] = true;
				break;
				}
			case CrossRenderer::WindowManager::WindowEventType::KeyReleased:
				{
				io.KeysDown[(uint32_t) Event.EventData.KeyReleased.Key] = false;
				break;
				}
			case CrossRenderer::WindowManager::WindowEventType::TextInput:
				{
				io.AddInputCharacter ( Event.EventData.TextInput.Text[0] );
				break;
				}
			default:
				break;
			}
		}
	if ( io.WantCaptureMouse )
		{
		switch ( Event.EventType )
			{
			case CrossRenderer::WindowManager::WindowEventType::ButtonPressed:
				{
				io.MouseDown[Event.EventData.ButtonPressed.Button] = true;
				break;
				}
			case CrossRenderer::WindowManager::WindowEventType::ButtonReleased:
				{
				io.MouseDown[Event.EventData.ButtonReleased.Button] = false;
				break;
				}
			case CrossRenderer::WindowManager::WindowEventType::MouseWheel:
				{
				io.MouseWheel = Event.EventData.MouseWheel.Amount.y;
				io.MouseWheelH = Event.EventData.MouseWheel.Amount.x;
				break;
				}
			default:
				break;
			}
		}
	switch ( Event.EventType )
		{
		case CrossRenderer::WindowManager::WindowEventType::WindowResized:
			{
			CrossRenderer::SetFramebufferClearColor ( Framebuffer, glm::vec4 ( 0.5f, 0.5f, 0.5f, 1.0f ) );
			io.DisplaySize.x = (float) Event.EventData.WindowResized.NewSize.x;
			io.DisplaySize.y = (float) Event.EventData.WindowResized.NewSize.y;
			break;
			}

		case CrossRenderer::WindowManager::WindowEventType::MouseMoved:
			{
			io.MousePos.x = (float) Event.EventData.MouseMoved.NewPosition.x;
			io.MousePos.y = (float) Event.EventData.MouseMoved.NewPosition.y;
			break;
			}
		default:
			break;
		}
	}

bool ImGuiTest::InitializeImGui ( void )
	{
	// Setup ImGui
	ImGui::CreateContext ();
	ImGuiIO &io = ImGui::GetIO ();
	ImGui::PushStyleVar ( ImGuiStyleVar_FrameRounding, 5 );
	//  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;        // We can honor GetMouseCursor() values (optional)
	//  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[]array.
	io.KeyMap[ImGuiKey_Tab] = (int) CrossRenderer::WindowManager::KeyCode::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = (int) CrossRenderer::WindowManager::KeyCode::Left;
	io.KeyMap[ImGuiKey_RightArrow] = (int) CrossRenderer::WindowManager::KeyCode::Right;
	io.KeyMap[ImGuiKey_UpArrow] = (int) CrossRenderer::WindowManager::KeyCode::Up;
	io.KeyMap[ImGuiKey_DownArrow] = (int) CrossRenderer::WindowManager::KeyCode::Down;
	io.KeyMap[ImGuiKey_PageUp] = (int) CrossRenderer::WindowManager::KeyCode::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = (int) CrossRenderer::WindowManager::KeyCode::PageDown;
	io.KeyMap[ImGuiKey_Home] = (int) CrossRenderer::WindowManager::KeyCode::Home;
	io.KeyMap[ImGuiKey_End] = (int) CrossRenderer::WindowManager::KeyCode::End;
	io.KeyMap[ImGuiKey_Insert] = (int) CrossRenderer::WindowManager::KeyCode::Insert;
	io.KeyMap[ImGuiKey_Delete] = (int) CrossRenderer::WindowManager::KeyCode::Delete;
	io.KeyMap[ImGuiKey_Backspace] = (int) CrossRenderer::WindowManager::KeyCode::Backspace;
	io.KeyMap[ImGuiKey_Space] = (int) CrossRenderer::WindowManager::KeyCode::Space;
	io.KeyMap[ImGuiKey_Enter] = (int) CrossRenderer::WindowManager::KeyCode::Enter;
	io.KeyMap[ImGuiKey_Escape] = (int) CrossRenderer::WindowManager::KeyCode::Escape;
	io.KeyMap[ImGuiKey_KeyPadEnter] = (int) CrossRenderer::WindowManager::KeyCode::KeypadEnter;
	io.KeyMap[ImGuiKey_A] = (int) CrossRenderer::WindowManager::KeyCode::A;
	io.KeyMap[ImGuiKey_C] = (int) CrossRenderer::WindowManager::KeyCode::C;
	io.KeyMap[ImGuiKey_V] = (int) CrossRenderer::WindowManager::KeyCode::V;
	io.KeyMap[ImGuiKey_X] = (int) CrossRenderer::WindowManager::KeyCode::X;
	io.KeyMap[ImGuiKey_Y] = (int) CrossRenderer::WindowManager::KeyCode::Y;
	io.KeyMap[ImGuiKey_Z] = (int) CrossRenderer::WindowManager::KeyCode::Z;

	io.DisplaySize.x = (float) CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () ).x;
	io.DisplaySize.y = (float) CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () ).y;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Create text atlas texture
	int width, height;
	unsigned char *pixels = nullptr;
	io.Fonts->GetTexDataAsRGBA32 ( &pixels, &width, &height );
	CrossRenderer::TextureDescriptor NewTextureDescriptor;
	NewTextureDescriptor.Dimensions = glm::uvec2 ( width, height );
	NewTextureDescriptor.Format = CrossRenderer::PixelFormat::RedGreenBlueAlpha8888;
	ImGuiData.FontTexture = CrossRenderer::CreateTexture ( NewTextureDescriptor );
	if ( !ImGuiData.FontTexture )
		return false;
	CrossRenderer::Load2DTextureData ( ImGuiData.FontTexture, CrossRenderer::PixelFormat::RedGreenBlueAlpha8888, pixels );

	// Load imgui shader
	ImGuiData.ShaderHandle = LoadShader (
							std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/ImGui.vert" ), "",
							std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/ImGui.frag" ) );
	if ( !ImGuiData.ShaderHandle )
		{
		CrossRenderer::DeleteTexture ( ImGuiData.FontTexture );
		return false;
		}

	ImGuiData.ShaderMVPUniform = CrossRenderer::GetShaderUniformHandle ( ImGuiData.ShaderHandle, "u_MVPMatrix" );
	ImGuiData.ShaderTextureUniform = CrossRenderer::GetShaderUniformHandle ( ImGuiData.ShaderHandle, "u_Texture" );
	ImGuiData.PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiData.ShaderHandle, "a_Position" );
	ImGuiData.TexCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiData.ShaderHandle, "a_TexCoord" );
	ImGuiData.ColorShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( ImGuiData.ShaderHandle, "a_Color" );

	io.Fonts->TexID = (ImTextureID) ImGuiData.FontTexture.GetKey ();

	ImGuiData.LastDataBufferSize = 0;
	ImGuiData.LastIndexBufferSize = 0;

	ImGuiData.IndexBufferStream.BufferHandle = ImGuiData.IndexBufferHandle;
	ImGuiData.IndexBufferStream.ComponentsPerElement = 1;
	ImGuiData.IndexBufferStream.ComponentType = ( sizeof ( ImDrawIdx ) == 2 ? CrossRenderer::ShaderBufferComponentType::Unsigned16 : CrossRenderer::ShaderBufferComponentType::Unsigned32 );
	ImGuiData.IndexBufferStream.NormalizeData = false;
	ImGuiData.IndexBufferStream.StartOffset = 0;
	ImGuiData.IndexBufferStream.Stride = sizeof ( ImDrawIdx );

	ImGuiData.VertexStream.BufferHandle = ImGuiData.DataBufferHandle;
	ImGuiData.VertexStream.ComponentsPerElement = 2;
	ImGuiData.VertexStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
	ImGuiData.VertexStream.NormalizeData = false;
	ImGuiData.VertexStream.StartOffset = offsetof ( ImDrawVert, pos );
	ImGuiData.VertexStream.Stride = sizeof ( ImDrawVert );

	ImGuiData.TexCoordStream.BufferHandle = ImGuiData.DataBufferHandle;
	ImGuiData.TexCoordStream.ComponentsPerElement = 2;
	ImGuiData.TexCoordStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
	ImGuiData.TexCoordStream.NormalizeData = false;
	ImGuiData.TexCoordStream.StartOffset = offsetof ( ImDrawVert, uv );
	ImGuiData.TexCoordStream.Stride = sizeof ( ImDrawVert );

	ImGuiData.ColorStream.BufferHandle = ImGuiData.DataBufferHandle;
	ImGuiData.ColorStream.ComponentsPerElement = 4;
	ImGuiData.ColorStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Unsigned8;
	ImGuiData.ColorStream.NormalizeData = true;
	ImGuiData.ColorStream.StartOffset = offsetof ( ImDrawVert, col );
	ImGuiData.ColorStream.Stride = sizeof ( ImDrawVert );

	ImGuiData.DeferredRendering = true;

	return true;
	}

void ImGuiTest::EndImGuiFrame ( void )
	{
	ImGui::DockSpaceOverViewport ( ImGui::GetMainViewport (), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode );
	ImGui::EndFrame ();

	// Start rendering
	ImGuiIO &io = ImGui::GetIO ();

	io.DisplaySize.x = (float) CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () ).x;
	io.DisplaySize.y = (float) CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () ).y;

	ImGui::Render ();

	ImDrawData *DrawData = ImGui::GetDrawData ();
	int fb_width = (int) ( DrawData->DisplaySize.x * DrawData->FramebufferScale.x );
	int fb_height = (int) ( DrawData->DisplaySize.y * DrawData->FramebufferScale.y );
	if ( fb_width <= 0 || fb_height <= 0 )
		return;

	ImGuiData.RenderCommands.clear ();
	if ( ImGuiData.DeferredRendering )
		{
		// Adapt buffer sizes if needed
		size_t TotalVertexCount = 0;
		size_t TotalIndexCount = 0;
		for ( int CommandListIndex = 0; CommandListIndex < DrawData->CmdListsCount; ++CommandListIndex )
			{
			TotalVertexCount += DrawData->CmdLists[CommandListIndex]->VtxBuffer.size ();
			TotalIndexCount += DrawData->CmdLists[CommandListIndex]->IdxBuffer.size ();
			}

		if ( TotalVertexCount * sizeof ( ImDrawVert ) > ImGuiData.LastDataBufferSize )
			{
			if ( ImGuiData.DataBufferHandle )
				CrossRenderer::DeleteShaderBuffer ( ImGuiData.DataBufferHandle );
			ImGuiData.DataBufferHandle = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( nullptr, TotalVertexCount * sizeof ( ImDrawVert ), CrossRenderer::ShaderBufferType::Array, CrossRenderer::ShaderBufferAccessType::Stream ) );
			ImGuiData.LastDataBufferSize = TotalVertexCount * sizeof ( ImDrawVert );
			ImGuiData.VertexStream.BufferHandle = ImGuiData.TexCoordStream.BufferHandle = ImGuiData.ColorStream.BufferHandle = ImGuiData.DataBufferHandle;
			}

		if ( TotalIndexCount * sizeof ( ImDrawIdx ) > ImGuiData.LastIndexBufferSize )
			{
			if ( ImGuiData.IndexBufferHandle )
				CrossRenderer::DeleteShaderBuffer ( ImGuiData.IndexBufferHandle );
			ImGuiData.IndexBufferHandle = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( nullptr, TotalIndexCount * sizeof ( ImDrawIdx ), CrossRenderer::ShaderBufferType::Array, CrossRenderer::ShaderBufferAccessType::Stream ) );
			ImGuiData.LastIndexBufferSize = TotalIndexCount * sizeof ( ImDrawIdx );
			ImGuiData.IndexBufferStream.BufferHandle = ImGuiData.IndexBufferHandle;
			}
		// Nothing to render
		if ( ( TotalIndexCount == 0 ) || ( TotalVertexCount == 0 ) )
			return;

		// Upload both buffers to GPU
		size_t VertexBufferOffset = 0, IndexBufferOffset = 0;
		uint8_t *VertexBufferPointer = (uint8_t *) CrossRenderer::MapShaderBuffer ( ImGuiData.DataBufferHandle, CrossRenderer::ShaderBufferMapAccessType::WriteOnly );
		uint8_t *IndexBufferPointer = (uint8_t *) CrossRenderer::MapShaderBuffer ( ImGuiData.IndexBufferHandle, CrossRenderer::ShaderBufferMapAccessType::WriteOnly );
		for ( int CommandListIndex = 0; CommandListIndex < DrawData->CmdListsCount; ++CommandListIndex )
			{
			const ImDrawList *CommandList = DrawData->CmdLists[CommandListIndex];
			memcpy ( VertexBufferPointer + VertexBufferOffset, CommandList->VtxBuffer.Data, sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size () );
			memcpy ( IndexBufferPointer + IndexBufferOffset, CommandList->IdxBuffer.Data, sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size () );
			VertexBufferOffset += sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size ();
			IndexBufferOffset += sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size ();
			}
		CrossRenderer::UnmapShaderBuffer ( ImGuiData.DataBufferHandle );
		CrossRenderer::UnmapShaderBuffer ( ImGuiData.IndexBufferHandle );

		// Create render commands
		VertexBufferOffset = 0;
		IndexBufferOffset = 0;

		for ( int CommandListIndex = 0; CommandListIndex < DrawData->CmdListsCount; ++CommandListIndex )
			{
			const ImDrawList *CommandList = DrawData->CmdLists[CommandListIndex];

			CrossRenderer::RenderCommand Command;
			Command.Shader = ImGuiData.ShaderHandle;
			Command.Primitive = CrossRenderer::PrimitiveType::TriangleList;
			Command.IndexBufferStream = ImGuiData.IndexBufferStream;
			Command.IndexBufferStream.StartOffset = IndexBufferOffset;
			Command.InstanceCount = 1;
			CrossRenderer::ShaderBufferDataStream VertexStream = ImGuiData.VertexStream;
			VertexStream.StartOffset = offsetof ( ImDrawVert, pos ) + VertexBufferOffset;
			CrossRenderer::ShaderBufferDataStream TexCoordStream = ImGuiData.TexCoordStream;
			TexCoordStream.StartOffset = offsetof ( ImDrawVert, uv ) + VertexBufferOffset;
			CrossRenderer::ShaderBufferDataStream ColorStream = ImGuiData.ColorStream;
			ColorStream.StartOffset = offsetof ( ImDrawVert, col ) + VertexBufferOffset;
			Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiData.PositionShaderAttribute, VertexStream ) );
			Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiData.TexCoordShaderAttribute, TexCoordStream ) );
			Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiData.ColorShaderAttribute, ColorStream ) );

			Command.State.Blending.Set ( CrossRenderer::BlendMode::SourceAlpha, CrossRenderer::BlendMode::OneMinusSourceAlpha );

			for ( int CommandIndex = 0, StartIndex = 0; CommandIndex < CommandList->CmdBuffer.Size; ++CommandIndex )
				{
				const ImDrawCmd *ImGuiCommand = &CommandList->CmdBuffer[CommandIndex];

				ImVec2 clip_off = DrawData->DisplayPos;      // (0,0) unless using multi-viewports
				ImVec2 clip_scale = DrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
				ImVec4 clip_rect;
				clip_rect.x = ( ImGuiCommand->ClipRect.x - clip_off.x ) * clip_scale.x;
				clip_rect.y = ( ImGuiCommand->ClipRect.y - clip_off.y ) * clip_scale.y;
				clip_rect.z = ( ImGuiCommand->ClipRect.z - clip_off.x ) * clip_scale.x;
				clip_rect.w = ( ImGuiCommand->ClipRect.w - clip_off.y ) * clip_scale.y;

				float L = DrawData->DisplayPos.x;
				float R = DrawData->DisplayPos.x + DrawData->DisplaySize.x;
				float T = DrawData->DisplayPos.y;
				float B = DrawData->DisplayPos.y + DrawData->DisplaySize.y;

				Command.TextureBindings.clear ();
				Command.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( ImGuiData.ShaderTextureUniform,
													CrossRenderer::TextureBindSettings ( CrossRenderer::TextureHandle ( (intptr_t) ImGuiCommand->TextureId ) ) ) );
				Command.State.Scissor.Set ( glm::uvec2 ( clip_rect.x, fb_height - clip_rect.w ), glm::uvec2 ( clip_rect.z - clip_rect.x, clip_rect.w - clip_rect.y ) );
				Command.StartVertex = StartIndex;
				Command.VertexCount = ImGuiCommand->ElemCount;
				Command.UniformValues.clear ();
				Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( ImGuiData.ShaderMVPUniform, glm::ortho ( L, R, B, T ) ) );

				assert ( CrossRenderer::SanityCheckRenderCommand ( Command ) );
				ImGuiData.RenderCommands.push_back ( Command );
				StartIndex += ImGuiCommand->ElemCount;
				}
			VertexBufferOffset += sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size ();
			IndexBufferOffset += sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size ();
			}
		}
	else
		{
		for ( int CommandListIndex = 0; CommandListIndex < DrawData->CmdListsCount; ++CommandListIndex )
			{
			const ImDrawList *CommandList = DrawData->CmdLists[CommandListIndex];

			// Upload both buffers to GPU
            if ( sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size () > ImGuiData.LastDataBufferSize )
				{
				if ( ImGuiData.DataBufferHandle )
					CrossRenderer::DeleteShaderBuffer ( ImGuiData.DataBufferHandle );
				ImGuiData.DataBufferHandle = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( nullptr, sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size (), CrossRenderer::ShaderBufferType::Array, CrossRenderer::ShaderBufferAccessType::Stream ) );
				ImGuiData.LastDataBufferSize = sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size ();
				ImGuiData.VertexStream.BufferHandle = ImGuiData.TexCoordStream.BufferHandle = ImGuiData.ColorStream.BufferHandle = ImGuiData.DataBufferHandle;
				}
            if ( sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size () > ImGuiData.LastIndexBufferSize )
				{
				if ( ImGuiData.IndexBufferHandle )
					CrossRenderer::DeleteShaderBuffer ( ImGuiData.IndexBufferHandle );
				ImGuiData.IndexBufferHandle = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( nullptr, sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size (), CrossRenderer::ShaderBufferType::Array, CrossRenderer::ShaderBufferAccessType::Stream ) );
				ImGuiData.LastIndexBufferSize = sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size ();
				ImGuiData.IndexBufferStream.BufferHandle = ImGuiData.IndexBufferHandle;
				}

			CrossRenderer::ChangeShaderBufferContents ( ImGuiData.DataBufferHandle, 0, CommandList->VtxBuffer.Data, sizeof ( ImDrawVert ) * CommandList->VtxBuffer.size () );
			CrossRenderer::ChangeShaderBufferContents ( ImGuiData.IndexBufferHandle, 0, CommandList->IdxBuffer.Data, sizeof ( ImDrawIdx ) * CommandList->IdxBuffer.size () );

			CrossRenderer::RenderCommand Command;
			Command.Shader = ImGuiData.ShaderHandle;
			Command.Primitive = CrossRenderer::PrimitiveType::TriangleList;
			Command.IndexBufferStream = ImGuiData.IndexBufferStream;
			Command.InstanceCount = 1;
			Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiData.PositionShaderAttribute, ImGuiData.VertexStream ) );
			Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiData.TexCoordShaderAttribute, ImGuiData.TexCoordStream ) );
			Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( ImGuiData.ColorShaderAttribute, ImGuiData.ColorStream ) );

			Command.State.Blending.Set ( CrossRenderer::BlendMode::SourceAlpha, CrossRenderer::BlendMode::OneMinusSourceAlpha );

			for ( int CommandIndex = 0, StartIndex = 0; CommandIndex < CommandList->CmdBuffer.Size; ++CommandIndex )
				{
				const ImDrawCmd *ImGuiCommand = &CommandList->CmdBuffer[CommandIndex];

				ImVec2 clip_off = DrawData->DisplayPos;      // (0,0) unless using multi-viewports
				ImVec2 clip_scale = DrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
				ImVec4 clip_rect;
				clip_rect.x = ( ImGuiCommand->ClipRect.x - clip_off.x ) * clip_scale.x;
				clip_rect.y = ( ImGuiCommand->ClipRect.y - clip_off.y ) * clip_scale.y;
				clip_rect.z = ( ImGuiCommand->ClipRect.z - clip_off.x ) * clip_scale.x;
				clip_rect.w = ( ImGuiCommand->ClipRect.w - clip_off.y ) * clip_scale.y;

				float L = DrawData->DisplayPos.x;
				float R = DrawData->DisplayPos.x + DrawData->DisplaySize.x;
				float T = DrawData->DisplayPos.y;
				float B = DrawData->DisplayPos.y + DrawData->DisplaySize.y;

				Command.TextureBindings.clear ();
				Command.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( ImGuiData.ShaderTextureUniform,
													CrossRenderer::TextureBindSettings ( CrossRenderer::TextureHandle ( (intptr_t) ImGuiCommand->TextureId ) ) ) );
				Command.State.Scissor.Set ( glm::uvec2 ( clip_rect.x, fb_height - clip_rect.w ), glm::uvec2 ( clip_rect.z - clip_rect.x, clip_rect.w - clip_rect.y ) );
				Command.StartVertex = StartIndex;
				Command.VertexCount = ImGuiCommand->ElemCount;
				Command.UniformValues.clear ();
				Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( ImGuiData.ShaderMVPUniform, glm::ortho ( L, R, B, T ) ) );
				assert ( CrossRenderer::SanityCheckRenderCommand ( Command ) );
				CrossRenderer::RunCommand ( Command );
				StartIndex += ImGuiCommand->ElemCount;
				}
			}
		}
	}

int main ( void )
	{
	ImGuiTest Test;
	if ( Test.Initialize () == false )
		return -1;
	if ( Test.Run () == false )
		return -2;
	if ( Test.Shutdown () == false )
		return -3;
	return 0;
	}
