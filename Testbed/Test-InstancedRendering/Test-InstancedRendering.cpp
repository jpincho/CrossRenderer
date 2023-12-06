#include "Test-InstancedRendering.hpp"

bool InstancedRendering::SpecificInitialize ( void )
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
	ViewMatrix = glm::lookAt ( glm::vec3 ( 0.0f, 0.0f, -2.0f ), glm::zero<glm::vec3> (), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );

	CrossRenderer::ShaderBufferDescriptor DataDescriptor ( Vertices, sizeof ( Vertices ) );
	DataBufferHandle = CrossRenderer::CreateShaderBuffer ( DataDescriptor );
	CrossRenderer::ShaderBufferDescriptor IndexDescriptor ( Indices, sizeof ( Indices ) );
	IndexBufferHandle = CrossRenderer::CreateShaderBuffer ( IndexDescriptor );
	for ( unsigned Index = 0; Index < 100; ++Index )
		{
		Rotations[Index].x = (float) rand ();
		Rotations[Index].y = (float) rand ();
		Rotations[Index].z = (float) rand ();
		Rotations[Index] = glm::normalize ( Rotations[Index] );
		ModelMatrices[Index] = glm::identity<glm::mat4> ();
		}
	CrossRenderer::ShaderBufferDescriptor MatricesDescriptor ( ModelMatrices, sizeof ( glm::mat4 ) * 100 );
	MatricesBufferHandle = CrossRenderer::CreateShaderBuffer ( MatricesDescriptor );

	ShaderHandle = LoadShader (
	                   std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/InstancedRendering.vert" ), "",
	                   std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/InstancedRendering.frag" ) );
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

	CrossRenderer::ShaderBufferDataStream InstanceMatricesStream ( MatricesBufferHandle,
	        0,
	        sizeof ( glm::mat4 ),
	        CrossRenderer::ShaderBufferComponentType::Float,
	        4 );

	CrossRenderer::ShaderAttributeHandle PositionAttributeHandle, TexCoordAttributeHandle, InstanceMatricesAttributeHandle;
	CrossRenderer::ShaderUniformHandle TextureUniformHandle, ViewProjectionUniformHandle;
	GET_ATTRIBUTE ( PositionAttributeHandle, ShaderHandle, "a_VertexPosition" );
	GET_ATTRIBUTE ( TexCoordAttributeHandle, ShaderHandle, "a_TexCoord" );
	GET_ATTRIBUTE ( InstanceMatricesAttributeHandle, ShaderHandle, "a_InstanceMatrix" );
	GET_UNIFORM ( ViewProjectionUniformHandle, ShaderHandle, "u_VP" );
	GET_UNIFORM ( TextureUniformHandle, ShaderHandle, "u_Texture" );
	CrossRenderer::TextureBindSettings TextureBindSettings ( TextureHandle );

	RenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
	RenderCommand.Shader = ShaderHandle;
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( PositionAttributeHandle, PositionStream ) );
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( TexCoordAttributeHandle, TexCoordStream ) );
	RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( InstanceMatricesAttributeHandle, InstanceMatricesStream ) );
	RenderCommand.StartVertex = 0;
	RenderCommand.VertexCount = 36;
	RenderCommand.InstanceCount = 100;
	RenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniformHandle, TextureBindSettings ) );
	RenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( ViewProjectionUniformHandle, ProjectionMatrix ) );
	RenderCommand.IndexBufferStream = IndexStream;
	RenderCommand.State.DepthTest.Set ( CrossRenderer::DepthTestMode::Less );
	assert ( CrossRenderer::SanityCheckRenderCommand ( RenderCommand ) );
	return true;
	}

bool InstancedRendering::SpecificShutdown ( void )
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
	if ( MatricesBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( MatricesBufferHandle );
		MatricesBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
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

bool InstancedRendering::SpecificFrame ( const float TimeDelta )
	{
	glm::mat4 *MatricesPointer = (glm::mat4 *) CrossRenderer::MapShaderBuffer ( MatricesBufferHandle, CrossRenderer::ShaderBufferMapAccessType::WriteOnly );
	for ( unsigned Index = 0; Index < 100; ++Index )
		{
		glm::vec3 Offset ( -1.0f, -1.0f, -3.0f );
		Offset.x = (float) ( Index / 10 );
		Offset.x /= 5.0f;
		Offset.y = (float) ( Index % 10 );
		Offset.y /= 5.0f;
		Offset.x -= 1.0f;
		Offset.y -= 1.0f;
		Offset.x *= 1.5f;
		Offset.y *= 1.5f;
		Offset.z = -1.0f;
		//LOG_DEBUG ( "%u %s", Index, glm::to_string(Offset).c_str() );

		ModelMatrices[Index] = glm::rotate ( glm::quarter_pi<float> () * TimeDelta, Rotations[Index] ) * ModelMatrices[Index];
		MatricesPointer[Index] = glm::translate ( glm::identity<glm::mat4> (), Offset ) *
		                         ModelMatrices[Index] *
		                         glm::scale ( glm::vec3 ( 0.1f, 0.1f, 0.1f ) );
		}
	CrossRenderer::UnmapShaderBuffer ( MatricesBufferHandle );
	CrossRenderer::RunCommand ( RenderCommand );
	return true;
	}

void InstancedRendering::SpecificOnEvent ( const CrossRenderer::WindowManager::WindowEvent &Event )
	{
	switch ( Event.EventType )
		{
		case CrossRenderer::WindowManager::WindowEventType::WindowResized:
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
	InstancedRendering Test;
	if ( Test.Initialize () == false )
		return -1;
	if ( Test.Run () == false )
		return -2;
	if ( Test.Shutdown () == false )
		return -3;
	return 0;
	}
