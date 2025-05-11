#include <chrono>
#include <mutex>
#include <resource_management/resource_manager.hpp>
#include <typeindex>

using namespace engine::resource_management;

std::shared_ptr<ResourceManager> ResourceManager::_instance = nullptr;
std::mutex ResourceManager::_instanceMutex;

ResourceManager::ResourceManager(std::string path) : _assetPath(path) {
  this->_cleaningThread = std::thread([this]() {
    std::unique_lock<std::mutex> lock(this->_cleaningMutex);
    while (this->_isCleaning) {
      if (this->_cleaningCv.wait_for(lock, std::chrono::seconds(5), [this]() {
        return !this->_isCleaning;
      })) break;

      lock.unlock();
      this->cleanAllCache();
      lock.lock();
    }
  });
}

ResourceManager::~ResourceManager() {
  {
    std::lock_guard<std::mutex> loc(this->_cleaningMutex);
    this->_isCleaning = false;
  }
  this->_cleaningCv.notify_one();
  if (this->_cleaningThread.joinable()) this->_cleaningThread.join();
  this->_isCleaning = false;
}

std::shared_ptr<ResourceManager> ResourceManager::Get(std::string path) {
  if (path.empty() && ResourceManager::_instance == nullptr) return nullptr;
  std::lock_guard<std::mutex> lock(ResourceManager::_instanceMutex);
  if (ResourceManager::_instance == nullptr)
    ResourceManager::_instance = std::shared_ptr<ResourceManager>(new ResourceManager(path));
  return ResourceManager::_instance;
}

void ResourceManager::cleanCache(std::type_index type) {
  std::unique_lock<std::shared_mutex> lock(this->_cacheMutex);
  auto& typeCache = this->_cache[type];
  for (auto it = typeCache.begin(); it != typeCache.end();) {
    if (it->second.expired()) {
      typeCache.erase(it);
      continue;
    }
    it++;
  }
}

void ResourceManager::cleanAllCache() {
  for (auto& [type, map] : this->_cache) {
    cleanCache(type);
  }
}
