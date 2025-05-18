#pragma once

#include <platform/window.hpp>

#include <memory>
#include <unordered_map>

#include <string>
#include <stdexcept>

namespace engine::platform {
    enum class WindowAPI {
        GLFW,
        Win32,
        Cocoa
    };
    class WindowManager {
        public:
            static std::shared_ptr<WindowManager> getInstance();

            std::shared_ptr<Window> createWindow(int width, int height, const char* name, WindowAPI api = WindowAPI::GLFW);

            std::shared_ptr<Window> getWindow(const char* name) const;
            bool hasWindow(const char* name) const;
            size_t windowCount() const;

            void closeWindow(const char* name);
            void closeAllWindows();

        private:
            std::unordered_map<std::string, std::shared_ptr<Window>> _windows;
            static std::shared_ptr<WindowManager> _instance;
    };
}