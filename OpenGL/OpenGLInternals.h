#pragma once
#include <glad/glad.h>
#include "../RendererDefinitions.h"
#include "../ShaderBufferDefinitions.h"
#include "../TextureDefinitions.h"
#include "../ShaderDefinitions.h"
#include "../FramebufferDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
typedef struct
    {
    GLint OpenGLID;
    ShaderUniformValue CurrentValue;
    ShaderUniformType Type;
    std::string Name;
    } UniformInfo;

typedef struct
    {
    GLint OpenGLID;
    bool Enabled;
    ShaderAttributeType Type;
    std::string Name;
    } AttributeInfo;

typedef struct ShaderInfo
    {
    GLuint OpenGLID;
    std::vector <UniformInfo> Uniforms;
    std::vector <AttributeInfo> Attributes;
    } ShaderInfo;

typedef struct ShaderBufferInfo : public ShaderBufferDescriptor
    {
    GLuint OpenGLID;
    GLenum GLAccessType;
    GLenum GLBufferType;
    GLenum GLMappedAccessType;
    void *MappedPointer;
    ShaderBufferInfo &operator = ( const ShaderBufferDescriptor &other )
        {
        *( (ShaderBufferDescriptor *) this ) = other;
        return *this;
        }
    } ShaderBufferInfo;

typedef struct TextureInfo : public TextureDescriptor
    {
    GLuint OpenGLID;
    GLint GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
    TextureInfo &operator = ( const TextureDescriptor &other )
        {
        *( (TextureDescriptor *) this ) = other;
        return *this;
        }
    } TextureInfo;

typedef struct FramebufferInfo : public FramebufferDescriptor
    {
    GLuint OpenGLID;
    std::vector <TextureHandle> ColorTextures;
    TextureHandle DepthTexture;
    FramebufferInfo &operator = ( const FramebufferDescriptor &other )
        {
        *( (FramebufferDescriptor *) this ) = other;
        return *this;
        }
    } FramebufferInfo;

extern VectorizedContainer <ShaderBufferInfo, ShaderBufferTag> ShaderBuffers;
extern VectorizedContainer <TextureInfo, TextureTag> Textures;
extern VectorizedContainer <ShaderInfo, ShaderTag> Shaders;
extern VectorizedContainer <FramebufferInfo, FramebufferTag> Framebuffers;

void OpenGLMessageCallback ( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param );
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
GLenum Translate ( const ShaderBufferType Value );
GLenum Translate ( const PrimitiveType Value );
GLint Translate ( const TextureFilter Value );
GLint Translate ( const TextureWrapMode Value );
}
}
