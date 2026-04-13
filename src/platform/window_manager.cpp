#include <platform/window_manager.hpp>

using namespace engine::platform;

std::shared_ptr<WindowManager> WindowManager::_instance = nullptr;

std::shared_ptr<WindowManager> WindowManager::getInstance() {
    if (_instance == nullptr) {
        _instance = std::make_shared<WindowManager>();
    }

    return _instance;
}

std::shared_ptr<Window> WindowManager::createWindow(int width, int height, const char* name, WindowAPI api) {
    hasWindow(name);
    std::shared_ptr<Window> window;

    switch (api) {
        case WindowAPI::GLFW:
            window = std::make_shared<GLFWWindow>(width, height, name);
            break;
        
        case WindowAPI::Win32:
        #if defined(_WIN64)
            window = std::make_shared<Win32Window>(width, height, name);
        #else
            throw std::runtime_error("Win32 API is not supported on this platform");
        #endif
            break;

        case WindowAPI::Cocoa:
        #if defined(__APPLE__) && defined(__MACH__)
            window = std::make_shared<CocoaWindow>(width, height, name);
        #else
            throw std::runtime_error("Cocoa API is not supported on this platform");
        #endif
            break;
        
        default:
            throw std::runtime_error("Unknown Window API");
    }

    _windows[name] = window;

    return window;
}

std::shared_ptr<Window> WindowManager::getWindow(const char* name) const {
    const char* windowName = name;
    if (_windows.find(windowName) != _windows.end()) {
        return _windows.at(windowName);
    } else {
        throw std::runtime_error("Window not found");
    }
}

bool WindowManager::hasWindow(const char* name) const {
    if (_windows.find(name) != _windows.end()) {
        throw std::runtime_error("Window already exists");
        return true;
    }
}

size_t WindowManager::windowCount() const {
    return _windows.size();
}

void WindowManager::closeWindow(const char* name) {
    if (hasWindow(name)) {
        _windows[name]->closeWindow();
        _windows.erase(name);
    } else {
        throw std::runtime_error("Window not found");
    }
}

void WindowManager::closeAllWindows() {
    for (std::pair<std::string, std::shared_ptr<Window>> window : _windows) {
        window.second->closeWindow();
    }
    _windows.clear();
}