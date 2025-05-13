#pragma once

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

class MetalRenderer {
public:
    MetalRenderer(id<MTLDevice> device, CAMetalLayer* layer);
    void draw();

private:
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLRenderPipelineState> pipelineState;
    CAMetalLayer* layer;

    void buildPipeline();
};
