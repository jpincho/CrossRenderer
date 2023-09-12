#pragma once
#include "../TestBase.hpp"

class InstancedRendering : public TestBase
	{
	protected:
		CrossRenderer::ShaderBufferHandle DataBufferHandle, IndexBufferHandle, MatricesBufferHandle;
		CrossRenderer::ShaderHandle ShaderHandle;
		CrossRenderer::TextureHandle TextureHandle;
		CrossRenderer::RenderCommand RenderCommand;
		glm::mat4 ProjectionMatrix, ViewMatrix;
		glm::mat4 ModelMatrices[100];
		glm::vec3 Rotations[100];

		bool SpecificInitialize ( void ) override;
		bool SpecificShutdown ( void ) override;
		bool SpecificFrame ( const float TimeDelta ) override;
		void SpecificOnEvent ( const CrossRenderer::WindowManager::WindowEvent &Event ) override;
	};
