#include <platform/window.hpp>
#include <iostream>
#include <vector>
#include <resource_management/resources/shader.hpp>

using namespace engine::platform;

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    glfwSetWindowUserPointer(this->_window, this);
}

void OpenGLWindow::clear() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, _width, _height);
}

void OpenGLWindow::show() {
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

float OpenGLWindow::update() {
    return 1.0;
}

bool OpenGLWindow::shouldClose() {
    return glfwWindowShouldClose(_window);
}
#endif