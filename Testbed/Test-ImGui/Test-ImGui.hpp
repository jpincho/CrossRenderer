#pragma once
#include "../TestBase.hpp"

class ImGuiTest : public TestBase
	{
	protected:
		struct
			{
			CrossRenderer::ShaderBufferHandle DataBufferHandle, IndexBufferHandle;
			CrossRenderer::ShaderHandle ShaderHandle;
			CrossRenderer::TextureHandle TextureHandle;
			CrossRenderer::RenderCommand RenderCommand;
			CrossRenderer::ShaderUniformHandle MVPUniformHandle;
			glm::mat4 ProjectionMatrix, ModelMatrix, ViewMatrix;
			}CubeData;

		struct
			{
			CrossRenderer::TextureHandle FontTexture;
			CrossRenderer::ShaderHandle ShaderHandle;
			CrossRenderer::ShaderUniformHandle ShaderMVPUniform, ShaderTextureUniform;
			CrossRenderer::ShaderAttributeHandle PositionShaderAttribute, TexCoordShaderAttribute, ColorShaderAttribute;
			size_t LastDataBufferSize = 0, LastIndexBufferSize = 0;
			CrossRenderer::ShaderBufferHandle DataBufferHandle, IndexBufferHandle;
			CrossRenderer::ShaderBufferDataStream IndexBufferStream, VertexStream, TexCoordStream, ColorStream;
			std::vector <CrossRenderer::RenderCommand> RenderCommands;
			bool DeferredRendering;
			}ImGuiData;

		bool SpecificInitialize ( void ) override;
		bool SpecificShutdown ( void ) override;
		bool SpecificFrame ( const float TimeDelta ) override;
		void SpecificOnEvent ( const CrossRenderer::WindowManager::WindowEvent &Event ) override;

		bool InitializeImGui ( void );
		void EndImGuiFrame ( void );
	};
