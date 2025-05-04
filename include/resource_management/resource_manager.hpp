#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <resource_management/resources/shader.hpp>


namespace engine::resource_management {

/**
 * @brief Manages loading, caching, and cleanup of various resources.
 *
 * The ResourceManager is a singleton that registers file-type-specific
 * resource loaders, provides access to loaded resources via caching,
 * and manages cleanup of unused resources in the background.
 */
class ResourceManager {
private:
  /**
   * @brief Singleton instance of the ResourceManager.
   */
  static std::shared_ptr<ResourceManager> _instance;
  /**
   * @brief Mutex to ensure thread-safe singleton initialization.
   */
  static std::mutex _instanceMutex;

  /**
   * @brief Mutex used to protect loader registration and access.
   */
  std::mutex _loaderMutex;
  /**
   * @brief Shared mutex for thread-safe access to the resource cache.
   */
  std::shared_mutex _cacheMutex;
  /**
   * @brief Mutex used to synchronize the background cache-cleaning thread.
   */
  std::mutex _cleaningMutex;
  /**
   * @brief Condition variable for signaling the cache-cleaning thread.
   */
  std::condition_variable _cleaningCv;

  /**
   * @brief Map of resource loaders indexed by file extension.
   */
  std::unordered_map<std::string, std::unique_ptr<ResourceLoader>> _loaders;
  /**
   * @brief Cache of loaded resources organized by file extension and path.
   *
   * The inner map stores weak pointers to resources by their file path.
   */
  std::unordered_map<std::string, std::unordered_map<std::string, std::weak_ptr<Resource>>> _cache;
  /**
   * @brief Background thread responsible for cleaning expired cache entries.
   */
  std::thread _cleaningThread;
  /**
   * @brief Flag used to control the execution of the cache-cleaning thread.
   */
  std::atomic<bool> _isCleaning = true;

  /**
   * @brief Private constructor to enforce singleton pattern.
   *
   * Initializes internal state and starts the background cleaning thread.
   */
  ResourceManager();

public:
  /**
   * @brief Deleted copy constructor to enforce singleton behavior.
   *
   * @param other Unused.
   */
  ResourceManager(ResourceManager& other) = delete;
  /**
   * @brief Destructor.
   *
   * Stops the background cleaning thread and cleans up resources.
   */
  ~ResourceManager();

  /**
   * @brief Provides access to the singleton instance of ResourceManager.
   *
   * Ensures thread-safe lazy initialization of the manager.
   *
   * @return Shared pointer to the singleton ResourceManager instance.
   */
  static std::shared_ptr<ResourceManager> Get();
  /**
   * @brief Registers a resource loader for a specific file extension.
   *
   * @param extension File extension handled by the loader.
   * @param loader Unique pointer to the ResourceLoader instance.
   */
  void registerLoader(std::string extension, std::unique_ptr<ResourceLoader> loader);
  /**
   * @brief Loads a resource from a given path using the appropriate loader.
   *
   * If a resource with the same path and extension is already cached and alive,
   * it is returned instead of loading again.
   *
   * @param path File path of the resource to load.
   * @param lazy If true, the resource is created but not immediately loaded.
   * @return Shared pointer to the loaded or cached resource.
   */
  std::shared_ptr<Resource> load(std::string path, bool hot_reload = false, bool lazy = false);
  /**
   * @brief Cleans up expired resources from the cache for a specific extension.
   *
   * This function removes any weak pointers that are no longer valid.
   *
   * @param extension The file extension to clean (e.g., ".png", ".shader").
   */
  void cleanCache(std::string extension);
  /**
   * @brief Cleans all expired resources from the entire cache.
   */
  void cleanAllCache();
  /**
   * @brief Deleted assignment operator to enforce singleton behavior.
   *
   * @param other Unused.
   */
  void operator=(const ResourceManager& other) = delete;
};  

}
