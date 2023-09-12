#include "Test-TexturedQuad.hpp"

bool TexturedQuadTest::SpecificInitialize ( void )
	{
	struct Vertex
		{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		};
	Vertex Vertices[4] =
		{
			{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}}
		};

	glm::uvec2 WindowSize = CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () );
	ProjectionMatrix = glm::perspective ( glm::half_pi<float> (), (float) WindowSize.x / (float) WindowSize.y, 0.1f, 10.0f );
	ModelMatrix = glm::identity<glm::mat4> ();
	ViewMatrix = glm::lookAt ( glm::vec3 ( 0.0f, 0.0f, -2.0f ), glm::zero<glm::vec3> (), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );

	CrossRenderer::ShaderBufferDescriptor Descriptor ( Vertices, sizeof ( Vertices ) );
	BufferHandle = CrossRenderer::CreateShaderBuffer ( Descriptor );
	ShaderHandle = LoadShader (
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/TexturedQuadTest.vert" ), "",
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/TexturedQuadTest.frag" ) );
	if ( !ShaderHandle )
		return false;

	TextureHandle = LoadTexture ( std::string ( TESTBED_DATA_PATH ) + std::string ( "/batman.jpg" ) );
	if ( !TextureHandle )
		return false;


	CrossRenderer::ShaderBufferDataStream PositionStream ( BufferHandle,
														  offsetof ( Vertex, Position ),
														  sizeof ( Vertex ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  3 );
	CrossRenderer::ShaderBufferDataStream TexCoordStream ( BufferHandle,
														  offsetof ( Vertex, TexCoord ),
														  sizeof ( Vertex ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  2 );

	CrossRenderer::ShaderAttributeHandle PositionAttributeHandle, TexCoordAttributeHandle;
	CrossRenderer::ShaderUniformHandle TextureUniformHandle, MVPUniformHandle;
	GET_ATTRIBUTE ( PositionAttributeHandle, ShaderHandle, "a_VertexPosition" );
	GET_ATTRIBUTE ( TexCoordAttributeHandle, ShaderHandle, "a_TexCoord" );
	GET_UNIFORM ( TextureUniformHandle, ShaderHandle, "u_Texture" );
	GET_UNIFORM ( MVPUniformHandle, ShaderHandle, "u_MVP" );

	CrossRenderer::TextureBindSettings TextureBindSettings ( TextureHandle );
	RenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
	RenderCommand.Shader = ShaderHandle;
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( PositionAttributeHandle, PositionStream ) );
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( TexCoordAttributeHandle, TexCoordStream ) );
	RenderCommand.StartVertex = 0;
	RenderCommand.VertexCount = 4;
	RenderCommand.InstanceCount = 1;
	RenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniformHandle, TextureBindSettings ) );
	RenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( MVPUniformHandle, ProjectionMatrix * ViewMatrix * ModelMatrix ) );
	assert ( CrossRenderer::SanityCheckRenderCommand ( RenderCommand ) );
	return true;
	}

bool TexturedQuadTest::SpecificShutdown ( void )
	{
	if ( BufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( BufferHandle );
		BufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
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

bool TexturedQuadTest::SpecificFrame ( const float )
	{
	CrossRenderer::RunCommand ( RenderCommand );
	return true;
	}

int main ( void )
	{
	TexturedQuadTest Test;
	if ( Test.Initialize () == false )
		return -1;
	if ( Test.Run () == false )
		return -2;
	if ( Test.Shutdown () == false )
		return -3;
	return 0;
	}
