#include <renderer/window.hpp>
#include <iostream>


using namespace engine::rendering;

#ifdef ENGINE_COMPILE_OPENGL
OpenGLWindow::OpenGLWindow(int width, int height, const char* name) : _width(width), _height(height), _windowName(name) {}

OpenGLWindow::~OpenGLWindow() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void OpenGLWindow::initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    _window = glfwCreateWindow(_width, _height, _windowName, nullptr, nullptr);

    glfwMakeContextCurrent(_window);

    glfwSetWindowUserPointer(this->_window, this);
}

float OpenGLWindow::update() {
    glfwSwapBuffers(_window);
    glfwPollEvents();

    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - _lastFrame;
    _lastFrame = currentFrame;

    return deltaTime;
}

bool OpenGLWindow::shouldClose() {
    return glfwWindowShouldClose(_window);
}
#endif

#ifdef ENGINE_COMPILE_VULKAN
VulkanWindow::VulkanWindow(int width, int height, const char* name) : _width(width), _height(height), _windowName(name) {}

VulkanWindow::~VulkanWindow(){
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void VulkanWindow::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(_width, _height, _windowName, nullptr, nullptr);
}

float VulkanWindow::update(){
    glfwPollEvents();
    return glfwGetTime();
}

bool VulkanWindow::shouldClose(){
    return glfwWindowShouldClose(_window);
}
#endif


#ifdef ENGINE_COMPILE_DIRECTX
ID3D11DeviceContext* DirectXWindow::context = nullptr;
ID3D11Device* DirectXWindow::device = nullptr;
DirectXWindow::DirectXWindow(int width, int height, const char* name) : _width(width), _height(height), _windowName(name) {}

DirectXWindow::~DirectXWindow(){
    DirectXWindow::context->Release();
    DirectXWindow::device->Release();
    renderTargetView->Release();
    swapChain->Release();
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void DirectXWindow::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(_width, _height, _windowName, nullptr, nullptr); //basic glfw window creation
    HWND hwnd = glfwGetWin32Window(_window); //changes the glfw window to a win32 window to output dx onto it
    
    DXGI_SWAP_CHAIN_DESC scd = {}; //initializes the backbuffer with default values
    scd.BufferCount = 1; //creates 1 backbuffer that will drawn on and shown then switches back and forht
    scd.BufferDesc.Width = _width; //width of the backbuffer
    scd.BufferDesc.Height = _height; //height of the backbuffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //color format for the window 8red 8green 8blue
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //specifes where the buffer will be used
    scd.OutputWindow = hwnd; //link to the output window duh
    scd.SampleDesc.Count = 1; //uhhhh weird ass anti aliasing  stuff
    scd.Windowed = TRUE; //forces it to be windowed mode
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //discards after swap i think

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &scd, &swapChain, &device, nullptr, &context
    ); //creates the dx device and the swap chain and the context

    ID3D11Texture2D* backBuffer; //creates the actual backbuffer (empty atm)
    swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&backBuffer); //gets the buffer pointer and casts into the backbuffer
    device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView); //this gives the destination as to where the gpu is supposed to draw (i think)
    backBuffer->Release(); //release the backbuffer (the render target view holds the refrence now so its useless to hold it any longer)
}


void DirectXWindow::clear(){
    context->OMSetRenderTargets(1, &renderTargetView, nullptr); //tells dx to actually draw on the RT

    float clearColor[] = {0.1f, 0.2f, 0.3f, 1.0f}; //the color format for the window (rgba) you can change it to whatever color
    context->ClearRenderTargetView(renderTargetView, clearColor); //clears the backbuffer which is now the RTV to the clearColor format (blue atm)

    //View Port Creation
    //view Port is simply an designated area in the created DX window that the context will draw on
    D3D11_VIEWPORT viewport = {};
    viewport.Width = _width;
    viewport.Height = _height;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    DirectXWindow::context->RSSetViewports(1, &viewport);
    //View Port Creation
}

void DirectXWindow::show(){
    glfwPollEvents();

    swapChain->Present(1,0); //presents the backbuffer after its "drawn" on
}

float DirectXWindow::update(){
    return 1.0;
}

bool DirectXWindow::shouldClose(){
    return glfwWindowShouldClose(_window);
}
#endif