#pragma once
#include "../TestBase.hpp"

class SkyboxTest : public TestBase
	{
	protected:
		CrossRenderer::ShaderBufferHandle SkyboxBufferHandle;
		CrossRenderer::ShaderHandle SkyboxShaderHandle;
		CrossRenderer::TextureHandle SkyboxTextureHandle;
		CrossRenderer::RenderCommand SkyboxRenderCommand;
		CrossRenderer::ShaderUniformHandle MVPUniformHandle;
		glm::mat4 ProjectionMatrix, ViewMatrix;

		glm::quat CameraOrientation;
		float CameraRotationSpeed;
		glm::ivec2 MouseMovement;
		float MouseSensitivity;

		bool SpecificInitialize ( void ) override;
		bool SpecificShutdown ( void ) override;
		bool SpecificFrame ( const float TimeDelta ) override;
		void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event ) override;
	};
