#pragma once
#include "../CrossRenderer.hpp"

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
#define GET_ATTRIBUTE(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderAttributeHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Attribute %s not found", NAME );\
                    return false;\
                    }

class TestBase : public CrossRenderer::WindowManager::EventListener
	{
	protected:
		bool ShouldQuit;
		CrossRenderer::FramebufferHandle Framebuffer;

	public:
		bool Initialize ( void );
		bool Shutdown ( void );
		bool Run ( void );
	private:
		void OnEvent ( const CrossRenderer::WindowManager::WindowEvent &Event );
	protected:
		virtual bool SpecificInitialize ( void ) = 0;
		virtual bool SpecificShutdown ( void ) = 0;
		virtual bool SpecificFrame ( const float TimeDelta ) = 0;
		virtual void SpecificOnEvent ( const CrossRenderer::WindowManager::WindowEvent & ) {};
	};

CrossRenderer::ShaderHandle LoadShader ( const std::string &VertexFile, const std::string &GeometryFile, const std::string &FragmentFile );
bool LoadFileContents ( const std::string &Path, std::vector <uint8_t> &Contents );
CrossRenderer::TextureHandle LoadTexture ( const std::string &ImageFile, const bool Flip = true );
CrossRenderer::TextureHandle LoadCubemapTexture ( const std::string ImageFile[6], const bool Flip = true );
double GetSeconds ( void );

