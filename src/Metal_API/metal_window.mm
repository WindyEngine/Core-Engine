#include "metal_window.hpp"
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import "MetalView.hpp"
#include <iostream>

MetalWindow::MetalWindow(int width, int height, const char* title)
    : window(nil), device(nil), commandQueue(nil), currentDrawable(nil), renderer(nullptr) {
    createWindow(width, height, title);  // Create window
    setupMetal();  // Set up Metal context
    renderer = new MetalRenderer(device, metalLayer);  // Initialize renderer with Metal device and layer
}

MetalWindow::~MetalWindow() {
    if (window) {
        [window close];  // Close the window on destruction
        window = nil;
    }
    delete renderer;  // Clean up the renderer
}

void MetalWindow::createWindow(int width, int height, const char* title) {
    @autoreleasepool {
        NSRect frame = NSMakeRect(0, 0, width, height);
        
        // Create the window as usual
        window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:(NSWindowStyleMaskTitled |
                                                         NSWindowStyleMaskClosable |
                                                         NSWindowStyleMaskResizable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
        [window setTitle:[NSString stringWithUTF8String:title]];

        // Create a MetalView and set it as the content view of the window
        MetalView* metalView = [[MetalView alloc] initWithFrame:frame];
        [window setContentView:metalView];  // This is where we set our custom view

        // Make the window visible
        [window makeKeyAndOrderFront:nil];
    }
}

void MetalWindow::setupMetal() {
    device = MTLCreateSystemDefaultDevice();
    commandQueue = [device newCommandQueue];

    // Get the MetalView from the window's content view
    MetalView* metalView = (MetalView*)[window contentView];
    
    // Now we can access the metalLayer in MetalView
    metalLayer = metalView.metalLayer;
    metalLayer.device = device;
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = YES;
    metalLayer.drawableSize = CGSizeMake(800, 600);
    metalLayer.contentsScale = [[NSScreen mainScreen] backingScaleFactor];
}


void MetalWindow::present() {
    @autoreleasepool {
        currentDrawable = [metalLayer nextDrawable];  // Get the next drawable from Metal layer
        if (!currentDrawable) return;

        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        [commandBuffer presentDrawable:currentDrawable];  // Present the drawable
        [commandBuffer commit];
    }
}

void MetalWindow::resize(int width, int height) {
    metalLayer.drawableSize = CGSizeMake(width, height);  // Update the drawable size
}

CAMetalLayer* MetalWindow::getLayer() {
    return metalLayer;
}

id<MTLDevice> MetalWindow::getDevice() {
    return device;
}

id<CAMetalDrawable> MetalWindow::getDrawable() {
    return currentDrawable;
}

void MetalWindow::clear() {
    @autoreleasepool {
        if (!currentDrawable) return;

        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];

        // Create the render pass descriptor correctly
        MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        
        MTLRenderPassColorAttachmentDescriptor* colorAttachment = [renderPassDescriptor colorAttachments][0];
        colorAttachment.texture = currentDrawable.texture;
        colorAttachment.loadAction = MTLLoadActionClear;
        colorAttachment.clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);  // Clear to black
        colorAttachment.storeAction = MTLStoreActionStore;

        id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [encoder endEncoding];

        [commandBuffer presentDrawable:currentDrawable];
        [commandBuffer commit];
    }
}

bool MetalWindow::shouldClose() {
    return !window || ![window isVisible];  // Check if the window is visible
}

void MetalWindow::update() {
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                             untilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]
                                                inMode:NSDefaultRunLoopMode
                                               dequeue:YES])) {
            [NSApp sendEvent:event];  // Process incoming events
        }
        [NSApp updateWindows];  // Update window state

        [window displayIfNeeded];  // Ensure the window is drawn

        renderer->draw();  // Update the renderer (call to draw the scene)
    }
}

