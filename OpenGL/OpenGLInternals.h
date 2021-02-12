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
    int OpenGLID;
    ShaderUniformValue CurrentValue;
    ShaderUniformType Type;
    std::string Name;
    } UniformInfo;

typedef struct
    {
    int OpenGLID;
    bool Enabled;
    ShaderAttributeType Type;
    std::string Name;
    } AttributeInfo;

typedef struct ShaderInfo
    {
    unsigned OpenGLID;
    std::vector <UniformInfo> Uniforms;
    std::vector <AttributeInfo> Attributes;
    } ShaderInfo;

typedef struct ShaderBufferInfo
    {
    unsigned OpenGLID;
    size_t DataSize;
    GLenum GLAccessType;
    GLenum GLBufferType;
    GLenum GLMappedAccessType;
    void *MappedPointer;
    ShaderBufferDescriptor::ShaderBufferFlags Flags;
    } ShaderBufferInfo;

typedef struct TextureInfo : public TextureDescriptor
    {
    unsigned OpenGLID;
    int GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
    } TextureInfo;

typedef struct FramebufferInfo
    {
    unsigned OpenGLID;
    std::vector <TextureHandle> ColorTextures;
    TextureHandle DepthTexture;
    glm::uvec2 Dimensions;
    glm::vec4 ClearColor;
    float ClearDepth;
    } FramebufferInfo;

extern VectorizedContainer <ShaderBufferInfo, ShaderBufferTag> ShaderBuffers;
extern VectorizedContainer <TextureInfo, TextureTag> Textures;
extern VectorizedContainer <ShaderInfo, ShaderTag> Shaders;
extern VectorizedContainer <FramebufferInfo, FramebufferTag> Framebuffers;
extern int MaxTextureUnits;
extern bool BufferStorageAvailable;

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
