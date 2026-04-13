#pragma once

#include <platform/window_manager.hpp>

#include <iostream>


using namespace engine::platform;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    if(uMsg == WM_DESTROY){
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


Win32Window::Win32Window(int width, int height, const char* name) : _width(width), _height(height), _name(name){
    _hInstance = GetModuleHandleA(nullptr);
    initWindow();
}

Win32Window::~Win32Window(){
    closeWindow();
    /*
    Win32Window::context->Release();
    Win32Window::device->Release();
    renderTargetView->Release();
    swapChain->Release();
    */
}

void Win32Window::initWindow(){
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = _hInstance;
    wc.lpszClassName = "DXWindow";
    RegisterClassExA(&wc);

    _hwnd = CreateWindowExA(
        0, "DXWindow", _name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
        CW_USEDEFAULT, _width, _height, nullptr, nullptr, _hInstance, nullptr
    );

    
    /*
    //window creation ends here everything below in this function has to do with rendering part should be removed later
    DXGI_SWAP_CHAIN_DESC scd = {}; //initializes the backbuffer with default values
    scd.BufferCount = 1; //creates 1 backbuffer that will drawn on and shown then switches back and forht
    scd.BufferDesc.Width = _width; //width of the backbuffer
    scd.BufferDesc.Height = _height; //height of the backbuffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //color format for the window 8red 8green 8blue
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //specifes where the buffer will be used
    scd.OutputWindow = _hwnd; //link to the output window duh
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
    */
}

/*
void Win32Window::clear(){
    
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
*/

void Win32Window::showWindow(){
    ShowWindow(this->_hwnd, SW_SHOW);
    //swapChain->Present(1,0); //presents the backbuffer after its "drawn" on
}

void Win32Window::hideWindow(){
    ShowWindow(this->_hwnd, SW_HIDE);
}

void Win32Window::closeWindow(){
    PostMessage(this->_hwnd, WM_CLOSE, 0, 0);
}

bool Win32Window::shouldClose() const {
    MSG msg = {};
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if(msg.message == WM_QUIT) return true;
    }
    return false;
}

void Win32Window::setName(const char* name){
    this->_name = name;
    if(this->_hwnd){
        SetWindowTextA(this->_hwnd, _name);
    }
}

char* Win32Window::getName() const {
    std::cout << "Window Name set to: " << this->_name << std::endl;
    return const_cast<char*>(this->_name);
}

std::pair<int, int> Win32Window::getSize() const {
    return { this->_width, this->_height };
}

void Win32Window::resize(int width, int height) {
    this->_width = width;
    this->_height = height;
    if (this->_hwnd) {
        SetWindowPos(_hwnd, nullptr, 0, 0, this->_width, this->_height, SWP_NOMOVE | SWP_NOZORDER);
    }
}

void* Win32Window::getWindow() const {
    return this->_hwnd;
}
