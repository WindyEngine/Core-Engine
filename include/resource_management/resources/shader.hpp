#pragma once

#include <memory>
#include <resource_management/resources/resource.hpp>
#include <resource_management/resources/file.hpp>
#include <platform/window.hpp>

namespace engine::resource_management {

class ShaderLayout {
  public:
    std::string name;
    int index, size, stride, offset;
    
};

class Shader : public Resource {
public:
  std::shared_ptr<File> vertex_shader;
  std::shared_ptr<File> fragment_shader;

  Shader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name = "", bool lazy = true);
  ~Shader() = default;

  void linkDependencies() override;

  virtual void useShader() {this->load();};
};

#ifdef ENGINE_COMPILE_OPENGL
class OpenGLShader : public Shader {
private:
  unsigned int vertex_shader_id;
  unsigned int fragment_shader_id;
  unsigned int shader_program_id;

  void load() override;
  void unload() override;

public:
  OpenGLShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name = "", bool lazy = true);
  ~OpenGLShader();

  void useShader() override;
};
#endif

#ifdef ENGINE_COMPILE_VULKAN
class VulkanShader : public Shader {
private:
  void load() override;
  void unload() override;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkRenderPass renderPass = VK_NULL_HANDLE;
  VkCommandPool commandPool = VK_NULL_HANDLE;
  static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


public:
  VulkanShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name = "", bool lazy = true);
  ~VulkanShader();
  void useShader() override;
};
#endif

#ifdef ENGINE_COMPILE_METAL
class MetalShader : public Shader {
private:
  void load() override;
  void unload() override;

public:
  id<MTLLibrary> library;
  id<MTLFunction> vertex_function;
  id<MTLFunction> fragment_function;
  id<MTLRenderPipelineState> pipeline_state;

  MetalShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name = "", bool lazy = true);
  ~MetalShader();

  void useShader() override;
};
#endif

#ifdef ENGINE_COMPILE_DIRECTX
class DirectShader : public Shader {
  private:
    void load() override;
    void unload() override;
  public:
    void useShader() override;
    ID3D11InputLayout* inputLayout = nullptr;
    DirectShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name = "", bool lazy = true);
    ~DirectShader();
};
#endif

}
