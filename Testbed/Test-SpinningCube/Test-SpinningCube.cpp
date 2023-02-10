#include "Test-SpinningCube.hpp"

bool SpinningCubeTest::SpecificInitialize ( void )
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
	ProjectionMatrix = glm::perspective ( glm::half_pi<float> (), (float) WindowSize.x / (float) WindowSize.y, 0.1f, 10.0f );
	ModelMatrix = glm::identity<glm::mat4> ();
	ViewMatrix = glm::lookAt ( glm::vec3 ( 0.0f, 0.0f, -2.0f ), glm::zero<glm::vec3> (), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );

	CrossRenderer::ShaderBufferDescriptor DataDescriptor ( Vertices, sizeof ( Vertices ) );
	DataBufferHandle = CrossRenderer::CreateShaderBuffer ( DataDescriptor );
	CrossRenderer::ShaderBufferDescriptor IndexDescriptor ( Indices, sizeof ( Indices ) );
	IndexBufferHandle = CrossRenderer::CreateShaderBuffer ( IndexDescriptor );

	ShaderHandle = LoadShader (
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/SpinningCubeTest.vert" ), "",
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/SpinningCubeTest.frag" ) );
	if ( !ShaderHandle )
		return false;

	CrossRenderer::SetFramebufferClearColor ( Framebuffer, glm::vec4 ( 0.5f, 0.5f, 0.5f, 1.0f ) );
	TextureHandle = LoadTexture ( std::string ( TESTBED_DATA_PATH ) + std::string ( "/batman.jpg" ) );
	if ( !TextureHandle )
		return false;

	CrossRenderer::ShaderBufferDataStream PositionStream ( DataBufferHandle,
														  offsetof ( Vertex, Position ),
														  sizeof ( Vertex ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  3 );
	CrossRenderer::ShaderBufferDataStream TexCoordStream ( DataBufferHandle,
														  offsetof ( Vertex, TexCoord ),
														  sizeof ( Vertex ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  2 );
	CrossRenderer::ShaderBufferDataStream IndexStream ( IndexBufferHandle,
														0,
														sizeof ( uint16_t ),
														CrossRenderer::ShaderBufferComponentType::Unsigned16,
														1 );
	CrossRenderer::ShaderAttributeHandle PositionAttributeHandle = CrossRenderer::GetShaderAttributeHandle ( ShaderHandle, "a_VertexPosition" );
	CrossRenderer::ShaderAttributeHandle TexCoordAttributeHandle = CrossRenderer::GetShaderAttributeHandle ( ShaderHandle, "a_TexCoord" );
	CrossRenderer::ShaderUniformHandle TextureUniformHandle = CrossRenderer::GetShaderUniformHandle ( ShaderHandle, "u_Texture" );
	MVPUniformHandle = CrossRenderer::GetShaderUniformHandle ( ShaderHandle, "u_MVP" );
	CrossRenderer::TextureBindSettings TextureBindSettings ( TextureHandle );

	RenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
	RenderCommand.Shader = ShaderHandle;
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( PositionAttributeHandle, PositionStream ) );
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( TexCoordAttributeHandle, TexCoordStream ) );
	RenderCommand.StartVertex = 0;
	RenderCommand.VertexCount = 36;
	RenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniformHandle, TextureBindSettings ) );
	RenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( MVPUniformHandle, ProjectionMatrix * ViewMatrix * ModelMatrix ) );
	RenderCommand.IndexBufferStream = IndexStream;
	RenderCommand.State.DepthTest.Set ( CrossRenderer::DepthTestMode::Less );
	assert ( CrossRenderer::SanityCheckRenderCommand ( RenderCommand ) );
	return true;
	}

bool SpinningCubeTest::SpecificShutdown ( void )
	{
	if ( DataBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( DataBufferHandle );
		DataBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	if ( IndexBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( IndexBufferHandle );
		IndexBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	if ( ShaderHandle )
		{
		CrossRenderer::DeleteShader ( ShaderHandle );
		ShaderHandle = CrossRenderer::ShaderHandle::Invalid;
		}
	if ( TextureHandle )
		{
		CrossRenderer::DeleteTexture ( TextureHandle );
		TextureHandle = CrossRenderer::TextureHandle::Invalid;
		}
	return true;
	}

bool SpinningCubeTest::SpecificFrame ( const float TimeDelta )
	{
	ModelMatrix = glm::rotate ( glm::quarter_pi<float> () * TimeDelta, glm::vec3 ( 0.0f, 1.0f, 0.0f ) ) * ModelMatrix;
	RenderCommand.UniformValues[0].UniformValue = ProjectionMatrix * ViewMatrix * ModelMatrix;
	CrossRenderer::RunCommand ( RenderCommand );
	return true;
	}


void SpinningCubeTest::SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
	{
	switch ( Event.EventType )
		{
		case CrossRenderer::WindowEventType::WindowResized:
			{
			CrossRenderer::SetFramebufferClearColor ( Framebuffer, glm::vec4 ( 0.5f, 0.5f, 0.5f, 1.0f ) );
			break;
			}
		default:
			break;
		}
	}
int main ( void )
	{
	SpinningCubeTest Test;
	if ( Test.Initialize () == false )
		return -1;
	if ( Test.Run () == false )
		return -2;
	if ( Test.Shutdown () == false )
		return -3;
	return 0;
	}
