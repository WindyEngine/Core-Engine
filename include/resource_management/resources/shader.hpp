#pragma once

#include <memory>
#include <resource_management/resources/resource.hpp>

namespace engine::resource_management {

/**
 * @brief Represents a GPU shader resource (e.g., vertex/fragment shader).
 *
 * Provides logic for loading and unloading shader data from file.
 * This class inherits from the generic Resource interface.
 */
class Shader : public Resource {
public:
  /**
   * @brief Inherit constructor from Resource.
   *
   * Initializes a Shader with the given file path.
   */
  using Resource::Resource;

  /**
   * @brief Loads the shader from its file.
   *
   * Typically compiles shader code and prepares GPU resources.
   */
  void load() override;
  /**
   * @brief Unloads the shader and releases any GPU memory.
   */
  void unload() override;
};

/**
 * @brief Resource loader for Shader objects.
 *
 * Responsible for creating and optionally loading Shader resources
 * from a given file path.
 */
class ShaderLoader : public ResourceLoader {
public:
  /**
   * @brief Loads a Shader resource from the specified path.
   *
   * @param path Path to the shader file.
   * @param lazy If true, the shader is created but not loaded immediately.
   * @return Shared pointer to the created Shader resource.
   */
  std::shared_ptr<Resource> load(std::string path, bool lazy) override;
};

}
