#pragma once

namespace engine::renderer {
    class Renderer {
        public:
            virtual ~Renderer() = default;

            virtual void initRenderer() = 0;
            virtual void render() = 0;

            // Cleanup resources
            virtual void cleanup() = 0;

            // Resize the viewport
            virtual void resize(int width, int height) = 0;
    };

    class OpenGLAPI : public Renderer {
        public:
            void initRenderer() override;
            void render() override;
            void cleanup() override;
            void resize(int width, int height) override;

        private:
            int _width;
            int _height;
    };

    class VulkanAPI : public Renderer {
        public:
            void initRenderer() override;
            void render() override;
            void cleanup() override;
            void resize(int width, int height) override;

        private:
            int _width;
            int _height;
    };

    class D3D11API : public Renderer {
        public:
            void initRenderer() override;
            void render() override;
            void cleanup() override;
            void resize(int width, int height) override;

        private:
            int _width;
            int _height;
    };

    class MetalAPI : public Renderer {
        public:
            void initRenderer() override;
            void render() override;
            void cleanup() override;
            void resize(int width, int height) override;

        private:
            int _width;
            int _height;
    };
}