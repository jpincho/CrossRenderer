#pragma once
#include "GLMHeaders.hpp"
#include "PixelFormat.hpp"

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
		}

    FramebufferDescriptor ( const glm::uvec2 Dimensions )
        {
        ColorAttachments = 1;
        ColorAttachmentFormat = PixelFormat::RedGreenBlueAlpha8888;
        DepthEnabled = true;
        DepthFormat = PixelFormat::DepthComponent;
        StencilEnabled = false;
        this->Dimensions = Dimensions;
        ClearDepth = 1.0f;
        }
    bool DepthEnabled, StencilEnabled;
    PixelFormat ColorAttachmentFormat, DepthFormat, StencilFormat;
    uint8_t ColorAttachments;
    glm::uvec2 Dimensions;
    glm::vec4 ClearColor;
    float ClearDepth;
	};
}
