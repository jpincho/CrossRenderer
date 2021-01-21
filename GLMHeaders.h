#pragma once
#include "CrossRendererConfig.h"

#if defined ( CROSS_RENDERER_COMPILER_MSVC )
#pragma warning ( push )
#pragma warning ( disable:4201 4301 4310 )
#endif
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined ( CROSS_RENDERER_COMPILER_MSVC )
#pragma warning ( pop )
#endif