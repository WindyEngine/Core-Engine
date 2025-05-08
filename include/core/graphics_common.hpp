#pragma once

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif


#ifdef ENGINE_COMPILE_VULKAN
#include <vulkan/vulkan.h>
#endif

#ifdef ENGINE_COMPILE_DIRECTX
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifdef ENGINE_COMPILE_METAL
#ifdef __OBJC__
    #import <Cocoa/Cocoa.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    class NSWindow;
    class NSView;
    class CAMetalLayer;
#endif
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#endif

#if defined(ENGINE_COMPILE_OPENGL) || defined(ENGINE_COMPILE_VULKAN) || defined(ENGINE_COMPILE_DIRECTX)
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif
