#pragma once
#include "HandleTemplate.h"
#include "GLMHeaders.h"
#include "PixelFormat.h"

namespace CrossRenderer
{
struct FramebufferTag {};
typedef HandleTemplate <FramebufferTag> FramebufferHandle;

typedef struct FramebufferDescriptor
    {
    FramebufferDescriptor ( const glm::uvec2 Dimensions )
        {
        ColorAttachments = 1;
        ColorAttachmentFormat = PixelFormat::RedGreenBlueAlpha8888;
        DepthEnabled = true;
        DepthFormat = PixelFormat::DepthComponent;
        StencilEnabled = false;
        this->Dimensions = Dimensions;
        DefaultClearDepth = 1.0f;
        }
    bool DepthEnabled, StencilEnabled;
    PixelFormat ColorAttachmentFormat, DepthFormat, StencilFormat;
    uint8_t ColorAttachments;
    glm::uvec2 Dimensions;
    glm::vec4 DefaultClearColor;
    float DefaultClearDepth;
    } FramebufferDescriptor;

}
