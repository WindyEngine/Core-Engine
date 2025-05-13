#pragma once

#include <platform/window.hpp>
#include <resource_management/resource_manager.hpp>
#include <core/types.hpp>

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif

#ifdef ENGINE_COMPILE_OPENGL
void openGLTriangle();
#endif

#ifdef ENGINE_COMPILE_VULKAN
void VulkanTriangle();
#endif

#ifdef ENGINE_COMPILE_DIRECTX
void DirectXTriangle();
#endif