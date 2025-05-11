/// @file resource_manager.hpp
#pragma once

#include "resource_management/resources/resource.hpp"
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <resource_management/resources/shader.hpp>


namespace engine::resource_management {

class ResourceManager {
private:
  static std::shared_ptr<ResourceManager> _instance;
  static std::mutex _instanceMutex;

  std::mutex _loaderMutex;
  std::shared_mutex _cacheMutex;
  std::mutex _cleaningMutex;
  std::condition_variable _cleaningCv;

  std::unordered_map<std::type_index, std::unique_ptr<ResourceLoader>> _loaders;
  std::unordered_map<std::type_index, std::unordered_map<std::string, std::weak_ptr<Resource>>> _cache;
  std::thread _cleaningThread;
  std::atomic<bool> _isCleaning = true;
  std::string _assetPath;

  ResourceManager(std::string path);

public:
  ResourceManager(ResourceManager& other) = delete;
  ~ResourceManager();

  static std::shared_ptr<ResourceManager> Get(std::string path = "");

  template<typename T>
  void registerLoader(std::unique_ptr<ResourceLoader> loader) {
    std::lock_guard<std::mutex> lock(this->_loaderMutex);
    this->_loaders[typeid(T)] = std::move(loader);
  }
  template<typename T>
  ResourceHandle<T> load(std::string path, bool hot_reload = false, bool lazy = false) {
    std::filesystem::path fullPath(_assetPath + "/" + path);
    std::string normalizedPath = std::filesystem::absolute(fullPath).lexically_normal().string();

    {
      std::shared_lock<std::shared_mutex> readLock(this->_cacheMutex);
      auto& typeCache = this->_cache[typeid(T)];
      if (auto cached = typeCache[normalizedPath].lock()) {
        cached->setHotReload(hot_reload);
        return cached->cast<T>();
      }
    }

    std::unique_lock<std::shared_mutex> writeLock(this->_cacheMutex);
    auto& typeCache = this->_cache[typeid(T)];
    if (auto cached = typeCache[normalizedPath].lock()) {
      cached->setHotReload(hot_reload);
      return cached->cast<T>();
    }

    std::unique_lock<std::mutex> loaderLock(this->_loaderMutex);
    auto loaderIt = this->_loaders.find(typeid(T));
    if (loaderIt == this->_loaders.end()) return nullptr;

    ResourceHandle<Resource> resource = loaderIt->second->load(normalizedPath, lazy);
    resource->setHotReload(hot_reload);
    loaderLock.unlock();

    typeCache[normalizedPath] = resource;
    return resource->cast<T>();
  }

  void cleanCache(std::type_index type);
  void cleanAllCache();
  void operator=(const ResourceManager& other) = delete;
};  

}
