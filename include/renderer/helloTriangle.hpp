#pragma once

#include <renderer/window.hpp>
#include <resource_management/resource_manager.hpp>

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif

using namespace engine::core;
using namespace engine::rendering;
using namespace engine::resource_management;

#ifdef ENGINE_COMPILE_OPENGL
unsigned int VAO;

OpenGLShader shader(
    std::make_shared<File>("path/to/vertex_shader.vert"),
    std::make_shared<File>("path/to/fragment_shader.frag")
);

void openGLTriangleTest() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    
    glGenVertexArrays(1, &VAO);
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    shader.useShader();
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    DirectXWindow window(800, 600, "DX Window");
    window.initWindow();

    ID3D11Buffer* vertexBuffer = nullptr;

    DirectXWindow::device->CreateBuffer(&bufferDesc, &sd, &vertexBuffer);
    
    ResourceManager::config.hot_reload = true;
    ResourceManager::setGraphicsLoader(GraphicsAPI::DirectX);

    std::shared_ptr<Shader> shader = ResourceManager::loadShader("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/VertexShader.hlsl", "C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/PixelShader.hlsl");

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    DirectXWindow::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    DirectXWindow::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    shader->useShader();


    while(!window.shouldClose()){
        window.clear();
        DirectXWindow::context->Draw(3, 0);
        window.show();
    }

}
#endif