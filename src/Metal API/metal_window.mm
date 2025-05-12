#import "metal_window.hpp"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

@interface MetalView : NSView
@end

@implementation MetalView
+ (Class)layerClass {
    return [CAMetalLayer class];
}
@end

MetalWindow::MetalWindow(int width, int height, const char* title) {
    createWindow(width, height, title);
    setupMetal();
}

MetalWindow::~MetalWindow() {
    [window release];
}

void MetalWindow::createWindow(int width, int height, const char* title) {
    NSRect frame = NSMakeRect(100, 100, width, height);
    window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:(NSWindowStyleMaskTitled |
                                                     NSWindowStyleMaskClosable |
                                                     NSWindowStyleMaskResizable)
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    [window setTitle:[NSString stringWithUTF8String:title]];
    contentView = [[MetalView alloc] initWithFrame:frame];
    [window setContentView:contentView];
    [window makeKeyAndOrderFront:nil];
}

void MetalWindow::setupMetal() {
    device = MTLCreateSystemDefaultDevice();
    metalLayer = (CAMetalLayer*)[contentView layer];
    metalLayer.device = device;
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = YES;
    metalLayer.contentsScale = [window backingScaleFactor];
    metalLayer.drawableSize = contentView.bounds.size;

    commandQueue = [device newCommandQueue];
}

void MetalWindow::resize(int width, int height) {
    metalLayer.drawableSize = CGSizeMake(width, height);
}

void MetalWindow::present() {
    currentDrawable = [metalLayer nextDrawable];
    id<MTLCommandBuffer> buffer = [commandQueue commandBuffer];
    [buffer presentDrawable:currentDrawable];
    [buffer commit];
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
