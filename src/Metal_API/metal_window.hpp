#pragma once

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>  // Ensure this is imported
#else
typedef void NSWindow;
typedef void CAMetalLayer;
#endif

#include <Metal/Metal.h>
#include <string>
#include "metal_renderer.hpp"

class MetalWindow {
public:
    MetalWindow(int width, int height, const char* title);
    ~MetalWindow();
    
    void initWindow();
    void clear();
    bool shouldClose();
    void update();
    void present();
    void resize(int width, int height);
    
    CAMetalLayer* getLayer();  // Returns the CAMetalLayer object
    id<MTLDevice> getDevice();  // Returns the Metal device
    id<CAMetalDrawable> getDrawable();  // Returns the Metal drawable
    
private:
    NSWindow* window;  // Window pointer
    id<MTLDevice> device;  // Metal device pointer
    id<MTLCommandQueue> commandQueue;  // Command queue
    CAMetalLayer* metalLayer;  // Metal layer
    id<CAMetalDrawable> currentDrawable;  // Current drawable object
    MetalRenderer* renderer;  // Renderer for drawing

    void setupMetal();  // Setup Metal context
    void createWindow(int width, int height, const char* title);  // Create the window
};
