/// @file resource.hpp
#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <filewatch/FileWatch.hpp>


namespace engine::resource_management {

class Resource;

template<typename T>
using ResourceHandle = std::shared_ptr<T>;
using GenericResourceHandle = ResourceHandle<Resource>;

class ResourceEvent {
public:
  enum Value {
    Reload, Load, Unload
  };

  ResourceEvent(Value v);

  Value value();

  operator Value() const;

private:
  Value _value;
};

class Resource : public std::enable_shared_from_this<Resource> {
protected:
  using ResourceEventCallbackMap = std::unordered_map<
    ResourceEvent::Value,
  std::vector<std::function<void(ResourceEvent, GenericResourceHandle)>>>;

  ResourceEventCallbackMap _subscribers;
  std::vector<std::weak_ptr<Resource>> _dependencies;
  std::vector<std::weak_ptr<Resource>> _dependents;
  std::unique_ptr<filewatch::FileWatch<std::string>> _watcher = nullptr;
  std::chrono::steady_clock::time_point _lastReload = std::chrono::steady_clock::now();
  std::mutex _reloadMutex;
  bool _loaded = false;
  std::string _path;
  bool _hot_reload = false;

public:
  Resource(std::string path);
  virtual ~Resource() = default;

  virtual bool load() = 0;
  virtual bool unload() = 0;
  virtual void reload();
  virtual void save() = 0;
  virtual void propagateChange();

  void addDependency(ResourceHandle<Resource> dep);
  void addDependent(ResourceHandle<Resource> dep);
  void subscribe(ResourceEvent event, std::function<void(ResourceEvent, GenericResourceHandle)> callback);
  void emit(ResourceEvent event);
  template<typename T>
  ResourceHandle<T> cast() {
    return std::dynamic_pointer_cast<T>(shared_from_this());
  }

  void setHotReload(bool state);
  bool isHotReloading();
  bool isLoaded();
  std::string getPath();
  size_t getSize();
};

class ResourceLoader {
public:
  virtual ~ResourceLoader() = default;

  virtual ResourceHandle<Resource> load(std::string path, bool lazy) = 0;
  virtual void create(std::string path) = 0;
};

}
