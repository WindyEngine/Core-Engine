#pragma once

#include <platform/window.hpp>
#include <resource_management/resource_manager.hpp>

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif

using namespace engine::core;
using namespace engine::platform;
using namespace engine::resource_management;

#ifdef ENGINE_COMPILE_OPENGL

void openGLTriangle() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    
    OpenGLWindow window(800, 600, "OpenGL Window");
    window.initWindow();
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    ResourceManager::config.hot_reload = true;
    ResourceManager::setGraphicsLoader(GraphicsAPI::OpenGL);

    std::shared_ptr<Shader> shader = ResourceManager::loadShader("C:/Users/acer/Documents/My-Projects/Graphics/graphics/shaders/vertexShader.vert", "C:/Users/acer/Documents/My-Projects/Graphics/graphics/shaders/fragmentShader.frag");
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    while (!window.shouldClose()) {
        window.clear();
        shader->useShader();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        window.show();
    }

}
#endif

#ifdef ENGINE_COMPILE_VULKAN
void VulkanTriangle(){
    VulkanWindow window(800, 600, "Vulkan Window");
    window.initWindow();

    ResourceManager::config.hot_reload = true;
    ResourceManager::setGraphicsLoader(GraphicsAPI::Vulkan);

    std::shared_ptr<Shader> shader = ResourceManager::loadShader("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/shader.vert.spv", "C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/shader.frag.spv"); 
    shader->useShader();
    
    while (!window.shouldClose()) {
        glfwPollEvents();
        window.clear();
        window.show();  
    }
}
#endif

#ifdef ENGINE_COMPILE_DIRECTX
void DirectXTriangle(){
    struct vertex {
        float x, y;
    };

    const vertex vertices[] = {
        {0.0f, 0.5f},
        {0.5f, -0.5},
        {-0.5f, -0.5},
    };

    //Buffer desc setup START
    /*this setup a vertex buffer that will only be used by the gpu it holds the vertex data which will be used---
    later on by the pipline is the buffer*/
    D3D11_BUFFER_DESC bufferDesc = {}; //zeros the buffer desc
    bufferDesc.Usage = D3D11_USAGE_DEFAULT; //defualt useage of the buffer means it will live on the GPU
    bufferDesc.ByteWidth = sizeof(vertices); //gets the byte size of the vertices 
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //specfies that this buffer will be used for vertex
    bufferDesc.CPUAccessFlags = 0; //this disallows the CPU from accessing the buffer
    //Buffer desc setup END

    D3D11_SUBRESOURCE_DATA sd = {}; //zeros the subsource data
    sd.pSysMem = vertices; //this will tell dx to copy the vertices into the buffer when the buffer is created later on

    DirectXWindow window(800, 600, "DX Window");
    window.initWindow();

    ID3D11Buffer* vertexBuffer = nullptr;

    DirectXWindow::device->CreateBuffer(&bufferDesc, &sd, &vertexBuffer); //this line inserts the triangle into the GPU through creating a buffer with the rules set by bufferDesc and subsource data in mind
    
    ResourceManager::config.hot_reload = true;
    ResourceManager::setGraphicsLoader(GraphicsAPI::DirectX);

    std::shared_ptr<Shader> shader = ResourceManager::loadShader("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/VertexShader.hlsl", "C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/PixelShader.hlsl");

    UINT stride = sizeof(vertex); //this is the size of each individual vertex in bytes (8 bytes) 
    UINT offset = 0; //the starting point of the buffer
    DirectXWindow::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset); //tells dx that when its about to draw it should take 8bytes with starting point of 0 from the vertexBuffer
    DirectXWindow::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //tells the gpu how to interpet the data
    shader->useShader();


    while(!window.shouldClose()){
        window.clear();
        DirectXWindow::context->Draw(3, 0);
        window.show();
    }

}
#endif