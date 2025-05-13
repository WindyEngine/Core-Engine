// MetalView.hpp
#pragma once
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

// Declare a custom NSView that will hold the CAMetalLayer
@interface MetalView : NSView

// Property to hold the CAMetalLayer
@property (strong, nonatomic) CAMetalLayer* metalLayer;

@end
