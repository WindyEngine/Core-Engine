#import <Cocoa/Cocoa.h>
#import "metal_window.hpp"

int main() {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];

        // Create the MetalWindow object correctly
        MetalWindow* window = new MetalWindow(800, 600, "Metal Game Engine");

        // Print that the app is running
        NSLog(@"Metal Game Engine is running...");

        // Main loop
        while (!window->shouldClose()) {
            window->clear();  // Clear the screen before rendering
            window->update(); // Update events and rendering
        }

        // Print that the app is closed
        NSLog(@"Metal Game Engine has been closed.");

        // Clean up
        delete window;
    }
    return 0;
}
