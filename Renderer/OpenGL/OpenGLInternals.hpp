#pragma once
#include <glad/glad.h>
#include "../RendererDefinitions.hpp"
#include "../ShaderBufferDefinitions.hpp"
#include "../TextureDefinitions.hpp"
#include "../ShaderDefinitions.hpp"
#include "../FramebufferDefinitions.hpp"
#include "../Version.hpp"
#include "../Internal/VectorizedContainer.hpp"
#include "OpenGLStateCache.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
struct OpenGLInformationStruct
	{
	GLint MaxTextureUnits;
	Version OpenGLVersion, GLSLVersion;
	std::string OpenGLRendererString, OpenGLVendorString;
	std::vector <std::string> Extensions;
	bool DirectStateAccessEnabled;
	};

extern OpenGLInformationStruct OpenGLInformation;

struct UniformInfo
	{
	GLint OpenGLID;
	ShaderUniformValue CurrentValue;
	ShaderUniformType Type;
	std::string Name;
	};

struct AttributeInfo
	{
	GLint OpenGLID;
	bool Enabled;
	ShaderAttributeType Type;
	std::string Name;
	};

struct ShaderInfo
	{
	GLuint OpenGLID;
	std::vector <ShaderObjectHandle> AttachedShaderObjects;
	std::vector <UniformInfo> Uniforms;
	std::vector <AttributeInfo> Attributes;
	};

struct ShaderBufferInfo : public ShaderBufferDescriptor
	{
	GLuint OpenGLID;
	GLenum GLAccessType;
	GLenum GLBufferType;
	GLenum GLMappedAccessType;
	void *MappedPointer;
	ShaderBufferInfo &operator = ( const ShaderBufferDescriptor &other )
		{
		* ( ( ShaderBufferDescriptor * ) this ) = other;
		return *this;
		}
	};

struct TextureInfo : public TextureDescriptor
	{
	GLuint OpenGLID;
	GLint GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
	TextureInfo &operator = ( const TextureDescriptor &other )
		{
		* ( ( TextureDescriptor * ) this ) = other;
		return *this;
		}
	};

struct FramebufferInfo : public FramebufferDescriptor
	{
	GLuint OpenGLID;
	std::vector <TextureHandle> ColorTextures;
	TextureHandle DepthTexture;
	TextureHandle StencilTexture;
	FramebufferInfo &operator = ( const FramebufferDescriptor &other )
		{
		* ( ( FramebufferDescriptor * ) this ) = other;
		return *this;
		}
	};

struct ShaderObjectInfo
	{
	GLuint OpenGLID;
	std::string Code;
	ShaderObjectType Type;
	};

extern RenderState CurrentState;
extern FramebufferHandle CurrentBoundFramebuffer;
extern VectorizedContainer <ShaderBufferInfo> ShaderBuffers;
extern VectorizedContainer <TextureInfo> Textures;
extern VectorizedContainer <ShaderInfo> Shaders;
extern VectorizedContainer <ShaderObjectInfo> ShaderObjects;
extern VectorizedContainer <FramebufferInfo> Framebuffers;

void OpenGLMessageCallback ( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *user_param );
const char *StringifyOpenGL ( GLenum Value );
bool CheckError ( void );
GLenum Translate ( const DepthTestMode Value );
GLenum Translate ( const BlendMode Value );
GLenum Translate ( const StencilFunction Value );
GLenum Translate ( const StencilFailAction Value );
GLenum Translate ( const CullingMode Value );
GLenum Translate ( const CullingFaceWinding Value );
GLenum Translate ( const ShaderBufferComponentType Value );
GLenum Translate ( const ShaderBufferAccessType Value );
GLenum Translate ( const ShaderBufferMapAccessType Value );
GLenum Translate ( const ShaderBufferType Value );
GLenum Translate ( const PrimitiveType Value );
GLint Translate ( const TextureFilter Value );
GLint Translate ( const TextureWrapMode Value );
GLenum Translate ( const PolygonMode Value );
ShaderUniformType TranslateOpenGLUniformType ( GLenum Type );
GLenum TranslateUniformType ( ShaderUniformType Type );
uint8_t BytesPerPixel ( const PixelFormat Format );
}
}
