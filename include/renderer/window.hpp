#pragma once

#include <core/graphics_common.hpp>

namespace engine::rendering {

    class Window {
        public:
            virtual ~Window() = default;

            virtual void initWindow() = 0;
            virtual bool shouldClose() = 0;
            virtual float update() = 0;
            virtual void clear() = 0;
            virtual void show() = 0;
    };

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

        private:
            GLFWwindow* _window;
            int _width, _height;
            const char* _windowName;
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
            IDXGISwapChain* swapChain = nullptr;
            static ID3D11DeviceContext* context;
            static ID3D11Device* device;
            ID3D11RenderTargetView* renderTargetView = nullptr;
            DirectXWindow(int width, int height, const char* name);
            ~DirectXWindow() override;

            void initWindow() override;
            bool shouldClose() override;
            float update() override;
            void  clear() override;
            void  show() override;

        private:
            int _width;
            int _height;
            const char* _windowName;
            GLFWwindow* _window;
    };
#endif
}
