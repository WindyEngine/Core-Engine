#import <Cocoa/Cocoa.h>
#import "metal_window.hpp"

int main() {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];

        // Create the MetalWindow object correctly
        MetalWindow* window = new MetalWindow(800, 600, "Metal Game Engine");

        // Main loop
        while (!window->shouldClose()) {
            window->clear();
            window->update();
        }

        // Clean up
        delete window;
    }
    return 0;
}
