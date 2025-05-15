#pragma once

#include <memory>
#include <unordered_map>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <core/graphics_common.hpp>
#include <string>
#include <stdexcept>

namespace engine::platform {
    enum class WindowAPI {
        GLFW,
        Win32,
        Cocoa
    };

    class Window {
        public:
            virtual ~Window() = default;

            // Window operations
            virtual void initWindow() = 0;
            virtual void showWindow() = 0;
            virtual void hideWindow() = 0;
            virtual void closeWindow() = 0;
            virtual bool shouldClose() const = 0;

            // Properties functions
            virtual void setName(const char* name) = 0;
            virtual char* getName() const = 0;
            virtual std::pair<int, int> getSize() const = 0;
            virtual void resize(int width, int height) = 0;

            //
            virtual void* getWindow() const = 0;

    };

    class GLFWWindow : public Window {
        public:
            GLFWWindow(int width, int height, const char* name);
            ~GLFWWindow() override;

            void initWindow() override;
            void showWindow() override;
            void hideWindow() override;
            void closeWindow() override;
            bool shouldClose() const override;

            void setName(const char* name) override;
            char* getName() const override;
            std::pair<int, int> getSize() const override;
            void resize(int width, int height) override;

            void* getWindow() const override;

        private:
            GLFWwindow* _glfwWindow;
            int _width;
            int _height;
            const char* _name;
    };

    class Win32Window : public Window {
        public:
            Win32Window(int width, int height, const char* name);
            ~Win32Window() override;

            void initWindow() override;
            void showWindow() override;
            void hideWindow() override;
            void closeWindow() override;
            bool shouldClose() const override;

            void setName(const char* name) override;
            char* getName() const override;
            std::pair<int, int> getSize() const override;
            void resize(int width, int height) override;

            void* getWindow() const override;

        private:
            GLFWwindow* glfwWindow; // placeholder change to win32 window
            int _width;
            int _height;
            const char* _name;
    };

    class CocoaWindow : public Window {
        public:
            CocoaWindow(int width, int height, const char* name);
            ~CocoaWindow() override;

            void initWindow() override;
            void showWindow() override;
            void hideWindow() override;
            void closeWindow() override;
            bool shouldClose() const override;

            void setName(const char* name) override;
            char* getName() const override;
            std::pair<int, int> getSize() const override;
            void resize(int width, int height) override;

            void* getWindow() const override;

        private:
            GLFWwindow* glfwWindow; // placeholder change to Cocoa window
            int _width;
            int _height;
            const char* _name;
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