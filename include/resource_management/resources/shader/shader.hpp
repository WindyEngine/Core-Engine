/**
 * @file shader.hpp
 * @brief Declares the Shader resource class and its loader used in the engine's resource management system.
 *
 * This header defines a concrete implementation of the `Resource` base class for shaders.
 * It supports multiple output formats including HLSL, GLSL, MSL, and SPIR-V, and integrates with
 * the shader compiler backend.
 */
#pragma once

#include <resource_management/resources/shader/shader_compiler.hpp>


namespace engine::resource_management {

/**
 * @class Shader
 * @brief Represents a shader resource that can be compiled into multiple target shading languages.
 *
 * The `Shader` class is a concrete subclass of `Resource` and is responsible for managing shader source code.
 * It supports loading from disk, unloading, and saving operations, and integrates with the engine's shader
 * compiler to produce output in various shading languages, including:
 * - HLSL (High-Level Shading Language, used in DirectX)
 * - GLSL (OpenGL Shading Language)
 * - MSL (Metal Shading Language, used on Apple platforms)
 * - SPIR-V (Standard Portable Intermediate Representation for Vulkan)
 */
class Shader : public Resource {
private:
  /**
   * @brief Stores the raw source code of the shader loaded from disk.
   */
  std::string _sourceCode;

public:
  /**
   * @brief Static extension string used for shader resource file identification.
   *
   * This can be used by file systems or loaders to recognize shader files (e.g., ".shader").
   */
  const static std::string extension;

  /**
   * @brief Inherit base class constructor.
   *
   * The `Shader` constructor takes the same parameters as the base `Resource` class (name and path).
   */
  using Resource::Resource;

  /**
   * @brief Destructor for the Shader resource.
   *
   * Handles any cleanup necessary upon destruction of the shader object.
   */
  ~Shader();

  /**
   * @brief Retrieves the raw shader source code.
   *
   * @return A string containing the full shader source as loaded from disk.
   */
  std::string getSourceCode();

  /**
   * @brief Compiles the shader source to HLSL (DirectX).
   *
   * @param entryPoint The name of the shader entry point function.
   * @return A string containing the HLSL source code.
   */
  std::string getHLSL(std::string entryPoint);

  /**
   * @brief Compiles the shader source to GLSL (OpenGL).
   *
   * @param entryPoint The name of the shader entry point function.
   * @return A string containing the GLSL source code.
   */
  std::string getGLSL(std::string entryPoint);

  /**
   * @brief Compiles the shader source to MSL (Metal).
   *
   * @param entryPoint The name of the shader entry point function.
   * @return A string containing the MSL source code.
   */
  std::string getMSL(std::string entryPoint);

  /**
   * @brief Compiles the shader source to SPIR-V (Vulkan).
   *
   * @param entryPoint The name of the shader entry point function.
   * @return A vector of 32-bit unsigned integers representing the SPIR-V binary.
   */
  std::vector<uint32_t> getSPIRV(std::string entryPoint);

  /**
   * @brief Loads the shader from disk.
   *
   * Reads the shader source code from the specified file path into memory.
   *
   * @return `true` if the shader was successfully loaded, `false` otherwise.
   */
  bool load() override;

  /**
   * @brief Unloads the shader from memory.
   *
   * Frees the shader source code
   *
   * @return `true` if the shader was successfully unloaded, `false` otherwise.
   */
  bool unload() override;

  /**
   * @brief Saves the shader to disk.
   *
   * Writes the current shader source code back to the original file path.
   * This function is expected to be implemented for runtime editing workflows.
   */
  void save() override;
};

/**
 * @class ShaderLoader
 * @brief Responsible for loading and creating Shader resources.
 *
 * The `ShaderLoader` class implements the `ResourceLoader` interface and is designed to
 * handle all file I/O and initialization procedures specific to shader resources.
 * It provides the logic for loading a shader from disk and for creating new shader files.
 */
class ShaderLoader : public ResourceLoader {
public:
  /**
   * @brief Loads a shader from disk and returns it as a resource handle.
   *
   * @param name The logical name of the shader resource.
   * @param path The file path where the shader source code resides.
   * @param lazy If true, the shader is initialized but not fully compiled until first use.
   * @return A `ResourceHandle` to the loaded shader.
   */
  ResourceHandle<Resource> load(std::string path, bool lazy) override;

  /**
   * @brief Creates a new shader resource file at the given path.
   *
   * This function is used for authoring tools or runtime editors to create new shaders from scratch.
   *
   * @param path The file path where the new shader file should be created.
   */
  void create(std::string path) override;
};

} // namespace engine::resource_management
