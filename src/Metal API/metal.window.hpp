#pragma once

#ifdef __OBJC__
@class NSWindow;
@class CAMetalLayer;
@class MetalView;
#else
typedef void NSWindow;
typedef void CAMetalLayer;
typedef void MetalView;
#endif

#include <Metal/Metal.h>

class MetalWindow {
public:
    MetalWindow(int width, int height, const char* title);
    ~MetalWindow();

    void present();
    void resize(int width, int height);

    void* getLayer();
    id<MTLDevice> getDevice();
    id<CAMetalDrawable> getDrawable();

private:
    NSWindow* window;
    MetalView* contentView;
    CAMetalLayer* metalLayer;
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<CAMetalDrawable> currentDrawable;

    void createWindow(int width, int height, const char* title);
    void setupMetal();
};
