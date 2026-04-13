#pragma once


namespace engine::core {

/*
Enum class for specifying the available graphics APIs.
This enum can represent different graphics APIs like OpenGL, Vulkan, DirectX, or Metal.
The preprocessor directives are used to enable/disable specific APIs based on compilation flags.
*/

enum class GraphicsAPI {
#ifdef ENGINE_COMPILE_OPENGL
  OpenGL,
#endif
#ifdef ENGINE_COMPILE_VULKAN
  Vulkan,
#endif
#ifdef ENGINE_COMPILE_DIRECTX
  DirectX,
#endif
#ifdef ENGINE_COMPILE_METAL
  Metal,
#endif
};

}
