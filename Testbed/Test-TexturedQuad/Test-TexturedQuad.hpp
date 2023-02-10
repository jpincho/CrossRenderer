#pragma once
#include "../TestBase.hpp"

class TexturedQuadTest : public TestBase
	{
	protected:
		CrossRenderer::ShaderBufferHandle BufferHandle;
		CrossRenderer::ShaderHandle ShaderHandle;
		CrossRenderer::TextureHandle TextureHandle;
		CrossRenderer::RenderCommand RenderCommand;
		glm::mat4 ProjectionMatrix, ModelMatrix, ViewMatrix;

		bool SpecificInitialize ( void ) override;
		bool SpecificShutdown ( void ) override;
		bool SpecificFrame ( const float TimeDelta ) override;
	};
