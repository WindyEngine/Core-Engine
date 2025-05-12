#pragma once

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#else
typedef void NSWindow;
typedef void CAMetalLayer;
#endif

#include <Metal/Metal.h>
#include <string>

class MetalWindow {
public:
    MetalWindow(int width, int height, const char* title);
    ~MetalWindow();
    
    void initWindow();
    void loadShader(const std::string& source);
    void useShader(id<MTLRenderCommandEncoder> encoder);
    void clear();
    bool shouldClose();
    float update();
    void present();
    void resize(int width, int height);
    
private:
    NSWindow* window;                       // Pointer to the NSWindow instance (Objective-C)
    id<MTLDevice> device;                   // Metal device for rendering
    id<MTLCommandQueue> commandQueue;       // Command queue for Metal commands
    CAMetalLayer* metalLayer;               // Metal layer for the window's content
    id<CAMetalDrawable> currentDrawable;    // Drawable for presenting Metal content

    void setupMetal();                      // Setup the Metal device and resources
    void createWindow(int width, int height, const char* title);  // Create a macOS window
};
