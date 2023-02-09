#pragma once
#include <stdint.h>

namespace CrossRenderer
{
enum class RendererHandleTemplateType
	{
    RenderWindow,
	Texture,
	Shader,
	ShaderObject,
	ShaderUniform,
	ShaderAttribute,
	ShaderBuffer,
	Framebuffer
	};

template <RendererHandleTemplateType TypeTag>
class RendererHandleTemplate
    {
    private:
        intptr_t Key;
    public:
		RendererHandleTemplate ( void )
            {
            Key = -1;
            }
		explicit RendererHandleTemplate ( intptr_t NewKey )
            {
            Key = NewKey;
            }
		intptr_t GetKey ( void ) const
            {
            return Key;
            }
		bool operator == ( const RendererHandleTemplate <TypeTag> &Other ) const
            {
            return ( Key == Other.Key );
            }
		bool operator != ( const RendererHandleTemplate <TypeTag> &Other ) const
            {
            return ( Key != Other.Key );
            }
		bool operator < ( const RendererHandleTemplate <TypeTag> &Other ) const
            {
            return ( Key < Other.Key );
            }
		RendererHandleTemplate &operator = ( const intptr_t Other )
            {
            Key = Other;
			return *this;
            }
        explicit operator bool ( void ) const
            {
			return *this != Invalid;
            }
		static const RendererHandleTemplate<TypeTag> Invalid;
    };

template <RendererHandleTemplateType TypeTag>
const RendererHandleTemplate<TypeTag> RendererHandleTemplate<TypeTag>::Invalid;

typedef RendererHandleTemplate <RendererHandleTemplateType::RenderWindow> RenderWindowHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::Texture> TextureHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::Framebuffer> FramebufferHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::ShaderBuffer> ShaderBufferHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::Shader> ShaderHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::ShaderObject> ShaderObjectHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::ShaderUniform> ShaderUniformHandle;
typedef RendererHandleTemplate <RendererHandleTemplateType::ShaderAttribute> ShaderAttributeHandle;
}

#include <functional>
namespace std
{
template <CrossRenderer::RendererHandleTemplateType Type>
struct hash <CrossRenderer::RendererHandleTemplate <Type> >
    {
	size_t operator () ( CrossRenderer::RendererHandleTemplate <Type> const &object ) const
        {
		return object.GetKey ();
        }
    };
}
