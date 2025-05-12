#import <Cocoa/Cocoa.h>
#import "metal_window.hpp"

int main() {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        MetalWindow* window = new MetalWindow(800, 600, "Metal Game Engine");
        [app run];
        delete window;
    }
    return 0;
}