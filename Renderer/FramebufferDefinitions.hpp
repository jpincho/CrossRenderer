#pragma once
#include "../GLMHeaders.hpp"
#include "../PixelFormat.hpp"

namespace CrossRenderer
{
struct FramebufferDescriptor
	{
	FramebufferDescriptor ( void )
		{
		ColorAttachments = 1;
		ColorAttachmentFormat = PixelFormat::RedGreenBlueAlpha8888;
		DepthEnabled = true;
		DepthFormat = PixelFormat::DepthComponent;
		StencilEnabled = false;
		ClearDepth = 1.0f;
		ClearColor = glm::zero<glm::vec4> ();
		}

    FramebufferDescriptor ( const glm::uvec2 NewDimensions )
        {
        ColorAttachments = 1;
        ColorAttachmentFormat = PixelFormat::RedGreenBlueAlpha8888;
        DepthEnabled = true;
        DepthFormat = PixelFormat::DepthComponent;
        StencilEnabled = false;
        Dimensions = NewDimensions;
        ClearDepth = 1.0f;
		ClearColor = glm::zero<glm::vec4> ();
        }
    bool DepthEnabled, StencilEnabled;
    PixelFormat ColorAttachmentFormat, DepthFormat, StencilFormat;
    uint8_t ColorAttachments;
    glm::uvec2 Dimensions;
    glm::vec4 ClearColor;
    float ClearDepth;
	};
}
