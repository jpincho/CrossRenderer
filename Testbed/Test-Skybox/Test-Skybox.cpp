#include "Test-Skybox.hpp"

bool SkyboxTest::SpecificInitialize ( void )
	{
	glm::uvec2 WindowSize = CrossRenderer::WindowManager::GetWindowSize ( *CrossRenderer::WindowManager::WindowList.begin () );
	ProjectionMatrix = glm::perspective ( glm::half_pi<float> (), (float) WindowSize.x / (float) WindowSize.y, 0.1f, 10.0f );
	ViewMatrix = glm::lookAt ( glm::vec3 ( 0.0f, 0.0f, 0.0f ), glm::vec3 ( 0.0f, 0.0f, -1.0f ), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );

	glm::vec3 SkyboxVertices[] =
		{
		// positions
			{-1.0f,  1.0f, -1.0f},
			{-1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},

			{-1.0f, -1.0f,  1.0f},
			{-1.0f, -1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f},

			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},

			{-1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f},

			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f},

			{-1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f}
		};

	CrossRenderer::ShaderBufferDescriptor Descriptor ( SkyboxVertices, sizeof ( SkyboxVertices ) );
	SkyboxBufferHandle = CrossRenderer::CreateShaderBuffer ( Descriptor );

	SkyboxShaderHandle = LoadShader (
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/SkyboxTest.vert" ), "",
				std::string ( TEST_PATH ) + std::string ( "/Shaders/OpenGLCore/SkyboxTest.frag" ) );
	if ( !SkyboxShaderHandle )
		return false;

	std::string Textures[6] =
		{
		std::string ( TESTBED_DATA_PATH ) + std::string ( "/Skybox/posx.jpg" ),
		std::string ( TESTBED_DATA_PATH ) + std::string ( "/Skybox/negx.jpg" ),
		std::string ( TESTBED_DATA_PATH ) + std::string ( "/Skybox/posy.jpg" ),
		std::string ( TESTBED_DATA_PATH ) + std::string ( "/Skybox/negy.jpg" ),
		std::string ( TESTBED_DATA_PATH ) + std::string ( "/Skybox/posz.jpg" ),
		std::string ( TESTBED_DATA_PATH ) + std::string ( "/Skybox/negz.jpg" )
		};
	SkyboxTextureHandle = LoadCubemapTexture ( Textures, false );
	if ( !SkyboxTextureHandle )
		return false;

	CameraOrientation = glm::toQuat ( glm::lookAt ( glm::zero<glm::vec3> (), glm::vec3 ( 0, 0, -1.0f ), glm::vec3 ( 0, 1, 0 ) ) );

	CameraRotationSpeed = glm::pi <float> () / 8;
	MouseMovement = glm::zero <glm::ivec2> ();
	MouseSensitivity = 1.0f / 10.0f;

	CrossRenderer::ShaderBufferDataStream PositionStream ( SkyboxBufferHandle,
														  0,
														  sizeof ( glm::vec3 ),
														  CrossRenderer::ShaderBufferComponentType::Float,
														  3 );
	CrossRenderer::ShaderAttributeHandle PositionAttributeHandle = CrossRenderer::GetShaderAttributeHandle ( SkyboxShaderHandle, "a_VertexPosition" );
	CrossRenderer::ShaderUniformHandle TextureUniformHandle = CrossRenderer::GetShaderUniformHandle ( SkyboxShaderHandle, "u_SkyboxTexture" );
	CrossRenderer::TextureBindSettings TextureBindSettings ( SkyboxTextureHandle );
	MVPUniformHandle = CrossRenderer::GetShaderUniformHandle ( SkyboxShaderHandle, "u_MVP" );
	SkyboxRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
	SkyboxRenderCommand.Shader = SkyboxShaderHandle;
	SkyboxRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( PositionAttributeHandle, PositionStream ) );
	SkyboxRenderCommand.StartVertex = 0;
	SkyboxRenderCommand.VertexCount = 36;
	SkyboxRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniformHandle, TextureBindSettings ) );
	SkyboxRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( MVPUniformHandle, ProjectionMatrix * ViewMatrix ) );
	assert ( CrossRenderer::SanityCheckRenderCommand ( SkyboxRenderCommand ) );
	return true;
	}

bool SkyboxTest::SpecificShutdown ( void )
	{
	if ( SkyboxBufferHandle )
		{
		CrossRenderer::DeleteShaderBuffer ( SkyboxBufferHandle );
		SkyboxBufferHandle = CrossRenderer::ShaderBufferHandle::Invalid;
		}
	if ( SkyboxShaderHandle )
		{
		CrossRenderer::DeleteShader ( SkyboxShaderHandle );
		SkyboxShaderHandle = CrossRenderer::ShaderHandle::Invalid;
		}
	if ( SkyboxTextureHandle )
		{
		CrossRenderer::DeleteTexture ( SkyboxTextureHandle );
		SkyboxTextureHandle = CrossRenderer::TextureHandle::Invalid;
		}
	return true;
	}

bool SkyboxTest::SpecificFrame ( const float TimeDelta )
	{
	if ( MouseMovement != glm::zero<glm::ivec2> () )
		{
		glm::vec3 YVector = glm::vec3 ( 0.0f, 1.0f, 0.0f );
		CameraOrientation = glm::angleAxis ( MouseMovement.x * MouseSensitivity * CameraRotationSpeed * TimeDelta, YVector ) * CameraOrientation;

		/*glm::vec3 XVector = CameraOrientation * glm::vec3 (1.0f, 0.0f, 0.0f);
		CameraOrientation = glm::angleAxis ( -MouseMovement.y * MouseSensitivity * CameraRotationSpeed * TimeDelta, XVector ) * CameraOrientation;*/

		MouseMovement = glm::zero<glm::ivec2> ();
		ViewMatrix = glm::toMat4 ( CameraOrientation );
		}
	SkyboxRenderCommand.UniformValues[0].UniformValue = ProjectionMatrix * ViewMatrix;

	CrossRenderer::RunCommand ( SkyboxRenderCommand );
	return true;
	}

void SkyboxTest::SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
	{
	switch ( Event.EventType )
		{
		case CrossRenderer::WindowEventType::WindowResized:
			{
			ProjectionMatrix = glm::perspective ( glm::half_pi<float> (), (float) Event.EventData.WindowResized.NewSize.x / (float) Event.EventData.WindowResized.NewSize.y, 0.1f, 10.0f );
			break;
			}
		case CrossRenderer::WindowEventType::MouseMoved:
			{
			MouseMovement.x += Event.EventData.MouseMoved.Delta.x;
			MouseMovement.y += Event.EventData.MouseMoved.Delta.y;
			break;
			}
		case CrossRenderer::WindowEventType::KeyPressed:
			{
			if ( Event.EventData.KeyPressed.Key == CrossRenderer::KeyCode::R )
				ViewMatrix = glm::lookAt ( glm::zero<glm::vec3> (), glm::vec3 ( 0.0f, 0.0f, -1.0f ), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );
			break;
			}
		default:
			break;
		}
	}

int main ( void )
	{
	SkyboxTest Test;
	if ( Test.Initialize () == false )
		return -1;
	if ( Test.Run () == false )
		return -2;
	if ( Test.Shutdown () == false )
		return -3;
	return 0;
	}
