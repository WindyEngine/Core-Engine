#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <vector>

namespace engine::platform {

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
            int _width;
            int _height;
            const char* _name;

            HWND _hwnd;
            HINSTANCE _hInstance;
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
/*
#ifdef ENGINE_COMPILE_OPENGL
    class OpenGLWindow : public Window {
        public:
            OpenGLWindow(int width, int height, const char* name);
            ~OpenGLWindow() override;

            void initWindow() override;
            bool shouldClose() override;
            float update() override;
            void clear() override;
            void show() override;

        private:
            GLFWwindow* _window;
            int _width, _height;
            const char* _windowName;
            float _lastFrame;
    };
#endif

#ifdef ENGINE_COMPILE_VULKAN
    class VulkanWindow : public Window {
        public:
            VulkanWindow(int width, int height, const char* name);
            ~VulkanWindow() override;

            void initWindow() override;
            bool shouldClose() override;
            float update() override;
            void clear() override;
            void show() override;

            static VkDevice device;
            static VkExtent2D swapChainExtent;
            static VkPhysicalDevice physicalDevice;
            static VkCommandBuffer commandBuffer;
            static VkPipeline graphicsPipeline;
            static uint32_t graphicsQueueFamilyIndex;
            static VkBuffer vertexBuffer;
            static VkDeviceMemory vertexBufferMemory;
            static VkRenderPass renderPass;

        private:
            GLFWwindow* _window;
            int _width, _height;
            const char* _windowName;
            VkInstance instance;
            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            VkQueue graphicsQueue;
            VkDeviceSize bufferSize;
            VkSemaphore imageAvailableSemaphore;
            VkSemaphore renderFinishedSemaphore;
            VkFence inFlightFence;
            VkCommandPool commandPool;

            std::vector<VkFramebuffer> swapChainFramebuffers;
            std::vector<VkImageView> swapChainImageViews;

            uint32_t imageIndex = 0; 
    };
#endif

#ifdef ENGINE_COMPILE_METAL
    class MetalWindow : public Window {
        public:
            MetalWindow(int width, int height, const char* name);
            ~MetalWindow() override;

            void initWindow() override;
            bool shouldClose() override;
            float update() override;

        private:
            int _width;                  
            int _height;                 
            const char* _windowName;    

            NSWindow* _window;
            NSView* _contentView;
            CAMetalLayer* _metalLayer;
    };
#endif

#ifdef ENGINE_COMPILE_DIRECTX
    class DirectXWindow : public Window {
        public:
            DirectXWindow(int width, int height, const char* name);
            ~DirectXWindow() override;

            IDXGISwapChain* swapChain = nullptr;
            static ID3D11DeviceContext* context;
            static ID3D11Device* device;
            static ID3D11InputLayout* inputLayout;
            ID3D11RenderTargetView* renderTargetView = nullptr;

            void initWindow() override;
            bool shouldClose() override;
            float update() override;
            void  clear() override;
            void  show() override;

        private:
            int _width;
            int _height;
            const char* _windowName;

            HWND _hwnd;
            HINSTANCE _hInstance;
    };
#endif
*/
}
