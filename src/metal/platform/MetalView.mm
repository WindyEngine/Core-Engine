// MetalView.mm
#import "MetalView.hpp"
#import <QuartzCore/CAMetalLayer.h>

// Implement the custom MetalView
@implementation MetalView

// Initialize the MetalView with a frame (size)
- (instancetype)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Create a CAMetalLayer
        self.metalLayer = [CAMetalLayer layer];
        self.metalLayer.frame = self.bounds;  // Set the layer's frame to match the view
        self.layer = self.metalLayer;  // Set the NSView's layer to the CAMetalLayer
        self.wantsLayer = YES;  // Tell the view that it has a custom layer
    }
    return self;
}

@end
