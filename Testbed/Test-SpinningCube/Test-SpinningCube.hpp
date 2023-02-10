#pragma once
#include "../TestBase.hpp"

class SpinningCubeTest : public TestBase
	{
	protected:
		CrossRenderer::ShaderBufferHandle DataBufferHandle, IndexBufferHandle;
		CrossRenderer::ShaderHandle ShaderHandle;
		CrossRenderer::TextureHandle TextureHandle;
		CrossRenderer::RenderCommand RenderCommand;
		CrossRenderer::ShaderUniformHandle MVPUniformHandle;
		glm::mat4 ProjectionMatrix, ModelMatrix, ViewMatrix;

		bool SpecificInitialize ( void ) override;
		bool SpecificShutdown ( void ) override;
		bool SpecificFrame ( const float TimeDelta ) override;
		void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event ) override;
	};
