#include <chrono>
#include <filesystem>
#include <mutex>
#include <resource_management/resource_manager.hpp>
#include <utility>

using namespace engine::resource_management;

// Initialize the singleton instance and its mutex
std::shared_ptr<ResourceManager> ResourceManager::_instance = nullptr;
std::mutex ResourceManager::_instanceMutex;

// Constructor starts the background cleaning thread
ResourceManager::ResourceManager() {
  this->_cleaningThread = std::thread([this]() {
    std::unique_lock<std::mutex> lock(this->_cleaningMutex);
    while (this->_isCleaning) {
      // Wait for either 5 seconds or a stop signal
      if (this->_cleaningCv.wait_for(lock, std::chrono::seconds(5), [this]() {
        return !this->_isCleaning;
      })) break;

      lock.unlock(); // Unlock while cleaning to avoid blocking
      this->cleanAllCache();
      lock.lock();   // Re-lock for the next iteration
    }
  });
}

// Destructor stops the background thread and joins it
ResourceManager::~ResourceManager() {
  {
    std::lock_guard<std::mutex> loc(this->_cleaningMutex);
    this->_isCleaning = false;
  }
  this->_cleaningCv.notify_one();
  if (this->_cleaningThread.joinable()) this->_cleaningThread.join();
  this->_isCleaning = false;
}

// Returns the singleton instance of the ResourceManager
std::shared_ptr<ResourceManager> ResourceManager::Get() {
  std::lock_guard<std::mutex> lock(ResourceManager::_instanceMutex);
  if (ResourceManager::_instance == nullptr)
    ResourceManager::_instance = std::shared_ptr<ResourceManager>(new ResourceManager);
  return ResourceManager::_instance;
}

// Registers a loader for a given file extension
void ResourceManager::registerLoader(std::string extension, std::unique_ptr<ResourceLoader> loader) {
  std::lock_guard<std::mutex> lock(this->_loaderMutex);
  this->_loaders[extension] = std::move(loader);
}

// Loads a resource from the given path (returns cached if available)
std::shared_ptr<Resource> ResourceManager::load(std::string path, bool hot_reload, bool lazy) {
  // Normalize the path and extract the extension
  std::string normalizedPath = std::filesystem::absolute(path).lexically_normal().string();
  std::string extension = std::filesystem::absolute(path).extension().string();

  // First try reading the cache with a shared lock
  {
    std::shared_lock<std::shared_mutex> readLock(this->_cacheMutex);
    auto& typeCache = this->_cache[extension];
    if (auto cached = typeCache[normalizedPath].lock()) {
      cached->setHotReload(hot_reload);
      return cached;
    }
  }

  // Upgrade to a unique lock if we need to write to the cache
  std::unique_lock<std::shared_mutex> writeLock(this->_cacheMutex);
  auto& typeCache = this->_cache[extension];
  if (auto cached = typeCache[normalizedPath].lock()) {
    cached->setHotReload(hot_reload);
    return cached;
  }

  // Acquire loader lock and find the appropriate loader
  std::unique_lock<std::mutex> loaderLock(this->_loaderMutex);
  auto loaderIt = this->_loaders.find(extension);
  if (loaderIt == this->_loaders.end()) return nullptr;

  // Load the resource
  auto resource = loaderIt->second->load(normalizedPath, lazy);
  resource->setHotReload(hot_reload);
  loaderLock.unlock();

  // Store the resource in the cache and return it
  typeCache[normalizedPath] = resource;
  return resource;
}

// Removes expired resources from the cache for a specific extension
void ResourceManager::cleanCache(std::string extension) {
  std::unique_lock<std::shared_mutex> lock(this->_cacheMutex);
  auto& typeCache = this->_cache[extension];
  for (auto it = typeCache.begin(); it != typeCache.end();) {
    if (it->second.expired()) {
      typeCache.erase(it);  // Remove if no longer valid
      continue;
    }
    it++;
  }
}

// Cleans expired resources for all cached extensions
void ResourceManager::cleanAllCache() {
  for (auto& [type, map] : this->_cache) {
    cleanCache(type);
  }
}
