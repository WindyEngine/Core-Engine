#include <platform/window_manager.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

using namespace engine::platform;

GLFWWindow::GLFWWindow(int width, int height, const char* name) : _width(width), _height(height), _name(name) {
    initWindow();
}

GLFWWindow::~GLFWWindow() {
    closeWindow();
}

void GLFWWindow::initWindow() {
    // Window is already initialized in the constructor
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    #ifdef ENGINE_COMPILE_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    #endif
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);

    this->_glfwWindow = glfwCreateWindow(this->_width, this->_height, this->_name, nullptr, nullptr);
    if (!this->_glfwWindow) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

}

void GLFWWindow::showWindow() {
    if (this->_glfwWindow) {
        glfwShowWindow(this->_glfwWindow);
    }
}

void GLFWWindow::hideWindow() {
    if (this->_glfwWindow) {
        glfwHideWindow(this->_glfwWindow);
    }
}

void GLFWWindow::closeWindow() {
    if (this->_glfwWindow) {
        glfwDestroyWindow(this->_glfwWindow);
        this->_glfwWindow = nullptr;
    }
    glfwTerminate();
}

bool GLFWWindow::shouldClose() const {
    return glfwWindowShouldClose(this->_glfwWindow);
}

void GLFWWindow::setName(const char* name) {
    this->_name = name;
    if (this->_glfwWindow) {
        glfwSetWindowTitle(this->_glfwWindow, this->_name);
    }
}

char* GLFWWindow::getName() const {
    std::cout << "Window Name set to: " << this->_name << std::endl;
    return const_cast<char*>(this->_name);
}

std::pair<int, int> GLFWWindow::getSize() const {
    return { this->_width, this->_height };
}

void GLFWWindow::resize(int width, int height) {
    this->_width = width;
    this->_height = height;
    if (this->_glfwWindow) {
        glfwSetWindowSize(this->_glfwWindow, this->_width, this->_height);
    }
}

void* GLFWWindow::getWindow() const {
    return this->_glfwWindow;
}