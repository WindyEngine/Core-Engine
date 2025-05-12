#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#include "metal_window.hpp"
#include <iostream>

MetalWindow::MetalWindow(int width, int height, const char* title)
: window(nil), device(nil), commandQueue(nil), metalLayer(nil), currentDrawable(nil) {
    createWindow(width, height, title);
    setupMetal();
}

MetalWindow::~MetalWindow() {
    if (window) {
        [window close];
        window = nil;
    }

    // Clean up Metal resources
    if (commandQueue) {
        commandQueue = nil;
    }
    if (device) {
        device = nil;
    }
    if (metalLayer) {
        metalLayer = nil;
    }
    if (currentDrawable) {
        currentDrawable = nil;
    }
}

void MetalWindow::createWindow(int width, int height, const char* title) {
    @autoreleasepool {
        NSRect frame = NSMakeRect(0, 0, width, height);
        
        // Create window
        window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:(NSWindowStyleMaskTitled |
                                                         NSWindowStyleMaskClosable |
                                                         NSWindowStyleMaskResizable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
        [window setTitle:[NSString stringWithUTF8String:title]];
        [window makeKeyAndOrderFront:nil];
    }
}

void MetalWindow::setupMetal() {
    @autoreleasepool {
        // Create Metal device and command queue
        device = MTLCreateSystemDefaultDevice();
        if (!device) {
            std::cerr << "Metal is not supported on this device!" << std::endl;
            return;
        }
        
        commandQueue = [device newCommandQueue];

        // Set up Metal layer
        metalLayer = [CAMetalLayer layer];
        metalLayer.device = device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.framebufferOnly = YES;
        metalLayer.contentsScale = [[NSScreen mainScreen] backingScaleFactor];
        
        // Set the layer's size to match the window
        metalLayer.drawableSize = CGSizeMake(800, 600);
    }
}

void MetalWindow::present() {
    @autoreleasepool {
        currentDrawable = [metalLayer nextDrawable];
        if (!currentDrawable) return;

        // Create command buffer to present the drawable
        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        [commandBuffer presentDrawable:currentDrawable];
        [commandBuffer commit];
    }
}

void MetalWindow::resize(int width, int height) {
    @autoreleasepool {
        // Resize the drawable layer
        metalLayer.drawableSize = CGSizeMake(width, height);
    }
}

void* MetalWindow::getLayer() {
    return metalLayer;
}

id<MTLDevice> MetalWindow::getDevice() {
    return device;
}

id<CAMetalDrawable> MetalWindow::getDrawable() {
    return currentDrawable;
}

bool MetalWindow::shouldClose() {
    @autoreleasepool {
        return ![window isVisible];
    }
}

void MetalWindow::clear() {
   
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
    id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [encoder endEncoding];
    [commandBuffer commit];
    
}
