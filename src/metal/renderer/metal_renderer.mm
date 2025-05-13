#import "metal_renderer.hpp"
#import <simd/simd.h>

@implementation NSObject (MetalRendererHelper)
@end

MetalRenderer::MetalRenderer(id<MTLDevice> device, CAMetalLayer* layer)
: device(device), layer(layer) {
    commandQueue = [device newCommandQueue];
    buildPipeline();
}

void MetalRenderer::buildPipeline() {
    NSError* error = nil;

    // Shader source code (vertex and fragment)
    NSString* shaderSource = @"#include <metal_stdlib>\n"
                              "using namespace metal;\n"
                              "struct VertexIn {\n"
                              "    float3 position [[attribute(0)]];\n"
                              "};\n"
                              "vertex float4 vertex_main(const VertexIn in [[stage_in]]) {\n"
                              "    return float4(in.position, 1.0);\n"
                              "}\n"
                              "fragment float4 fragment_main() {\n"
                              "    return float4(1, 0, 0, 1);\n"
                              "}";

    // Create Metal library
    id<MTLLibrary> lib = [device newLibraryWithSource:shaderSource options:nil error:&error];
    if (error) {
        NSLog(@"Error creating Metal library: %@", error.localizedDescription);
        return;
    }

    // Load functions
    id<MTLFunction> vtx = [lib newFunctionWithName:@"vertex_main"];
    id<MTLFunction> frag = [lib newFunctionWithName:@"fragment_main"];

    // Set up the vertex descriptor
    MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
    
    // Describe the input layout for the vertex shader
    vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;  // vertex position as float3
    vertexDescriptor.attributes[0].offset = 0;
    vertexDescriptor.attributes[0].bufferIndex = 0;  // Buffer index for vertex data

    vertexDescriptor.layouts[0].stride = sizeof(simd_float3);  // 3 floats per vertex

    // Create the pipeline descriptor and assign the vertex descriptor
    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vtx;
    desc.fragmentFunction = frag;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.vertexDescriptor = vertexDescriptor;  // Assign the vertex descriptor to the pipeline

    // Create the pipeline state object
    pipelineState = [device newRenderPipelineStateWithDescriptor:desc error:&error];

    if (error) {
        NSLog(@"Error creating pipeline state: %@", error.localizedDescription);
    }
}

void MetalRenderer::draw() {
    // Get a drawable from the CAMetalLayer
    id<CAMetalDrawable> drawable = [layer nextDrawable];
    if (!drawable) {
        NSLog(@"Error: No drawable available");
        return;
    }

    // Define the vertices of a triangle
    static const simd_float3 vertices[] = {
        { 0.0f,  0.5f, 0.0f },
        {-0.5f, -0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f }
    };

    // Create the render pass descriptor
    MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
    passDesc.colorAttachments[0].texture = drawable.texture;
    passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
    passDesc.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);  // Clear color (black)
    passDesc.colorAttachments[0].storeAction = MTLStoreActionStore;

    // Create a command buffer
    id<MTLCommandBuffer> cmd = [commandQueue commandBuffer];

    // Create a render command encoder
    id<MTLRenderCommandEncoder> encoder = [cmd renderCommandEncoderWithDescriptor:passDesc];
    
    [encoder setRenderPipelineState:pipelineState];

    // Set the vertex buffer
    id<MTLBuffer> vertexBuffer = [device newBufferWithBytes:vertices
                                                   length:sizeof(vertices)
                                                  options:MTLResourceStorageModeShared];
    [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

    // Issue the draw command for a triangle
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];

    // End encoding
    [encoder endEncoding];

    // Present the drawable
    [cmd presentDrawable:drawable];
    [cmd commit];
}
