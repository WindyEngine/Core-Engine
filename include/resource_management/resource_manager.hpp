/**
 * @file ResourceManager.hpp
 * @brief Declares the central resource management system for handling game assets like shaders and textures.
 *
 * The `ResourceManager` class provides a centralized mechanism for loading, caching, tracking,
 * and unloading reusable engine resources such as shaders, textures, and more. It supports lazy loading,
 * hot reloading, automatic cache cleaning, and type-safe resource handling.
 */
#pragma once

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <shared_mutex>
#include <thread>
#include <typeindex>

#include <resource_management/resources/shader/shader.hpp>
#include <resource_management/resources/texture.hpp>


namespace engine::resource_management {

/**
 * @class ResourceManager
 * @brief Singleton responsible for managing all loadable resources in the engine.
 *
 * The `ResourceManager` handles the lifecycle of all resources by:
 * - Registering custom resource loaders for each type.
 * - Caching and reusing resource instances to prevent redundant loading.
 * - Supporting hot reloading and lazy loading.
 * - Automatically cleaning expired resources from cache in a background thread.
 *
 * Resources are stored and retrieved in a type-safe way using `std::type_index`.
 * The manager ensures thread safety using shared and exclusive locks to coordinate access to internal caches.
 *
 * @note This class should be accessed through `ResourceManager::Get()`.
 */
class ResourceManager {
private:
  /** @brief Singleton instance. */
  static std::shared_ptr<ResourceManager> _instance;

  /** @brief Mutex to guard singleton instantiation. */
  static std::mutex _instanceMutex;

  /** @brief Mutex for guarding loader registration and access. */
  std::mutex _loaderMutex;

  /** @brief Shared mutex for thread-safe read/write access to the resource cache. */
  std::shared_mutex _cacheMutex;

  /** @brief Mutex used during cache cleaning operations. */
  std::mutex _cleaningMutex;

  /** @brief Condition variable to signal background cache cleaning thread. */
  std::condition_variable _cleaningCv;

  /**
   * @brief Map of registered resource loaders indexed by type.
   *
   * Each loader knows how to load a specific type of `Resource`, such as `Shader` or `Texture`.
   */
  std::unordered_map<std::type_index, std::unique_ptr<ResourceLoader>> _loaders;

  /**
   * @brief In-memory cache of loaded resources.
   *
   * The cache is a two-level map indexed by type and absolute file path.
   * Each resource is held using a weak pointer to allow for automatic garbage collection.
   */
  std::unordered_map<std::type_index, std::unordered_map<std::string, std::weak_ptr<Resource>>> _cache;

  /** @brief Background thread used to automatically clean unused resources from cache. */
  std::thread _cleaningThread;

  /** @brief Flag indicating whether the cleaning thread should keep running. */
  std::atomic<bool> _isCleaning = true;

  /** @brief The root path under which all asset paths are resolved. */
  std::string _assetPath;

  /**
   * @brief Private constructor for singleton pattern.
   * @param path The root directory used for all asset file paths.
   */
  ResourceManager(std::string path);

public:
  /**
   * @brief Deleted copy constructor to enforce singleton behavior.
   */
  ResourceManager(ResourceManager& other) = delete;

  /**
   * @brief Destructor.
   *
   * Cleans up background threads and releases held resources.
   */
  ~ResourceManager();

  /**
   * @brief Returns the singleton instance of the `ResourceManager`.
   *
   * If the instance has not yet been created, it will be constructed with the provided path.
   * Subsequent calls must provide the same or an empty path.
   *
   * @param path The root asset directory path. Only used during the first call.
   * @return A shared pointer to the singleton `ResourceManager`.
   */
  static std::shared_ptr<ResourceManager> Get(std::string path = "");

  /**
   * @brief Registers a new loader for the given resource type `T`.
   *
   * The loader will be used to load resources of type `T` when requested through `load<T>()`.
   *
   * @tparam T The resource type (e.g., `Shader`, `Texture`).
   * @param loader A unique pointer to the loader implementation for type `T`.
   */
  template<typename T>
  void registerLoader(std::unique_ptr<ResourceLoader> loader) {
    std::lock_guard<std::mutex> lock(this->_loaderMutex);
    this->_loaders[typeid(T)] = std::move(loader);
  }

  /**
   * @brief Loads a resource of type `T` using its registered loader.
   *
   * If the resource is already cached, it returns the cached instance.
   * Otherwise, it invokes the appropriate loader and caches the result.
   *
   * @tparam T The resource type to load (must inherit from `Resource`).
   * @param path Relative path to the asset from the asset root directory.
   * @param name Optional name for the resource. If omitted, the filename (without extension) is used.
   * @param hot_reload Enables file watching and automatic reloading when the file changes.
   * @param lazy If true, the resource is created but not fully loaded until used.
   * @return A shared pointer to the loaded resource, or nullptr if loading failed.
   */
  template<typename T>
  ResourceHandle<T> load(std::string path, bool hot_reload = false, bool lazy = false) {
    // Combine asset root with input path, and normalize the result to an absolute clean path.
    std::filesystem::path fullPath(_assetPath + "/" + path);
    std::string normalizedPath = std::filesystem::absolute(fullPath).lexically_normal().string();

    {
      // Acquire a shared (read) lock to check if the resource already exists in cache.
      std::shared_lock<std::shared_mutex> readLock(this->_cacheMutex);
      auto& typeCache = this->_cache[typeid(T)];

      // Attempt to retrieve the cached resource from the type-specific cache.
      if (auto cached = typeCache[normalizedPath].lock()) {
        // If found and valid, update hot reload flag and return it.
        cached->setHotReload(hot_reload);
        return cached->cast<T>();
      }
    }

    // Acquire a unique (write) lock since we're about to modify the cache.
    std::unique_lock<std::shared_mutex> writeLock(this->_cacheMutex);
    auto& typeCache = this->_cache[typeid(T)];

    // Double-check in case the resource was inserted by another thread in the meantime.
    if (auto cached = typeCache[normalizedPath].lock()) {
      cached->setHotReload(hot_reload);
      return cached->cast<T>();
    }

    // Acquire a lock to access the loader map.
    std::unique_lock<std::mutex> loaderLock(this->_loaderMutex);

    // Look for the loader registered for the resource type T.
    auto loaderIt = this->_loaders.find(typeid(T));
    if (loaderIt == this->_loaders.end()) return nullptr; // No loader registered for this type, cannot load.

    // Use the loader to load the resource with the provided name and normalized path.
    ResourceHandle<Resource> resource = loaderIt->second->load(normalizedPath, lazy);
    resource->setHotReload(hot_reload);

    // Unlock loader mutex early to avoid unnecessary blocking.
    loaderLock.unlock();

    // Insert the newly loaded resource into the cache.
    typeCache[normalizedPath] = resource;

    // Return the resource cast to the correct type.
    return resource->cast<T>();
  }

  /**
   * @brief Manually cleans the resource cache for a specific type.
   *
   * This function removes expired (unused) weak pointers from the cache of type `type`.
   * It does not affect resources currently in use.
   *
   * @param type The type of the resource to clean (use `typeid(T)`).
   */
  void cleanCache(std::type_index type);

  /**
   * @brief Cleans all resource caches for all types.
   *
   * This method iterates through the entire cache and removes all expired entries,
   * freeing memory occupied by unused resources.
   */
  void cleanAllCache();

  /**
   * @brief Deleted copy assignment operator to enforce singleton behavior.
   */
  void operator=(const ResourceManager& other) = delete;
};  

} // namespace engine::resource_management
