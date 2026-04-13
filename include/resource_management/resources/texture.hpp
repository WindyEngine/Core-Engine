
/**
 * @file texture.hpp
 * @brief Declares the Texture resource class and its corresponding loader for managing GPU texture assets.
 *
 * This file defines a texture-specific subclass of the base `Resource` class in the resource management system.
 * It provides texture-specific implementations of the resource lifecycle (loading, unloading, saving) and integrates
 * with the resource management infrastructure. The associated `TextureLoader` class is responsible for instantiating
 * and initializing `Texture` instances from file paths.
 */
#pragma once

#include <resource_management/resources/resource.hpp>


namespace engine::resource_management {

/**
 * @class Texture
 * @brief Represents a 2D or 3D texture resource that can be loaded and managed within the engine.
 *
 * The `Texture` class inherits from the abstract base `Resource` class and implements the required
 * virtual functions for loading, unloading, and saving texture data. This class provides the fundamental
 * abstraction for texture resources such as diffuse maps, normal maps, specular maps, and more.
 *
 * It is intended to be used in conjunction with a graphics backend or renderer, where the actual image
 * data would be uploaded to GPU memory.
 *
 * @see Resource
 */
class Texture : public Resource {
public:
  /**
   * @brief Inherits constructor from Resource.
   *
   * Allows construction using the base class constructor, typically requiring a resource name and file path.
   */
  using Resource::Resource;

  /**
   * @brief Destructor for the Texture class.
   *
   * Ensures that any allocated resources are cleaned up when the texture is destroyed.
   */
  ~Texture();

  /**
   * @brief Loads the texture resource from the specified path.
   *
   * This function reads the texture file, decodes the image data (using stb_image),
   * This function is typically called when the resource manager first needs to use the texture.
   *
   * @return `true` if the texture was successfully loaded, `false` otherwise.
   */
  bool load() override;

  /**
   * @brief Unloads the texture from memory.
   *
   * This function releases any memory resources associated with the texture.
   * It is useful in memory-constrained environments or when performing hot reloads.
   *
   * @return `true` if the texture was successfully unloaded, `false` otherwise.
   */
  bool unload() override;

  /**
   * @brief Saves the texture resource to its associated file path.
   *
   * This function writes any changes made to the texture data back to disk.
   * In many engines, this function may be a no-op if textures are read-only at runtime.
   */
  void save() override;
};

/**
 * @class TextureLoader
 * @brief Responsible for instantiating and initializing `Texture` resources.
 *
 * The `TextureLoader` class is a concrete implementation of the abstract `ResourceLoader` interface.
 * It is responsible for creating and loading `Texture` objects from file paths, and optionally handling
 * lazy-loading strategies.
 *
 * This loader is registered with the resource manager and is invoked automatically when a `.png`, `.jpg`, `.dds`,
 * or other supported texture file is requested.
 *
 * @see ResourceLoader
 * @see Texture
 */
class TextureLoader : public ResourceLoader {
public:
  /**
   * @brief Loads a texture resource from the given path.
   *
   * @param name The logical name of the texture (e.g., "brick_diffuse").
   * @param path The filesystem path to the texture file.
   * @param lazy If `true`, the texture is registered but not loaded until used.
   * @return A shared pointer to the loaded `Texture` resource.
   */
  ResourceHandle<Resource> load(std::string path, bool lazy) override;

  /**
   * @brief Creates a new texture resource file at the specified path.
   *
   * This function may be used in tools or editors to generate an empty or default texture file on disk.
   *
   * @param path The destination file path where the texture should be created.
   */
  void create(std::string path) override;
};

} // namespace engine::resource_management
