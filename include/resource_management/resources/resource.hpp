#pragma once

#include <ctime>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <filewatch/FileWatch.hpp>


namespace engine::resource_management {

/**
 * @brief Types of events that a Resource can emit.
 */
enum class ResourceEvent {
  /**
   * @brief Indicates the resource has changed (e.g., reloaded).
   */
  Change
};

/**
 * @brief Abstract base class representing a generic loadable resource.
 *
 * A Resource tracks its own loading state, file path, dependencies, and dependents.
 * It supports event subscription for change propagation and must be subclassed
 * to provide specific loading and unloading logic.
 */
class Resource : public std::enable_shared_from_this<Resource> {
protected:
  /**
   * @brief Event subscribers organized by event type.
   */
  std::unordered_map<ResourceEvent, std::vector<std::function<void(std::shared_ptr<Resource>)>>> _subscribers;
  /**
   * @brief Resources this one depends on.
   */
  std::vector<std::weak_ptr<Resource>> _dependencies;
  /**
   * @brief Resources that depend on this resource.
   */
  std::vector<std::weak_ptr<Resource>> _dependents;
  /**
   * @brief File Watcher monitoring the source file
   */
  std::unique_ptr<filewatch::FileWatch<std::string>> _watcher = nullptr;

public:
  /**
   * @brief Filesystem path associated with the resource.
   */
  std::string path;
  /**
   * @brief Indicates whether the resource is currently loaded.
   */
  bool loaded = false;
  /**
   * @brief Indicates whether the resource is hot-reloaded.
   */
  bool hot_reload = false;

  /**
   * @brief Constructs a Resource with the given file path.
   *
   * @param path Path to the resource file.
   */
  Resource(std::string path);
  /**
   * @brief Virtual destructor for polymorphic deletion.
   */
  virtual ~Resource() = default;

  /**
   * @brief Loads the resource from its file.
   *
   * Must be implemented by derived classes to perform actual loading.
   */
  virtual void load() = 0;
  /**
   * @brief Unloads the resource and releases any held memory or GPU data.
   */
  virtual void unload() = 0;
  /**
   * @brief Reloads the resource, typically after external changes.
   */
  virtual void reload();
  /**
   * @brief Notifies all dependents that the resource has changed.
   */
  virtual void propagateChange();

  /**
   * @brief Adds a resource that this one depends on.
   *
   * @param dep Shared pointer to the dependency resource.
   */
  void addDependency(std::shared_ptr<Resource> dep);
  /**
   * @brief Registers a resource as a dependent of this one.
   *
   * @param dep Shared pointer to the dependent resource.
   */
  void addDependent(std::shared_ptr<Resource> dep);
  /**
   * @brief Subscribes a callback to a specific resource event.
   *
   * @param event The event type to subscribe to.
   * @param callback Function to be called when the event occurs.
   */
  void subscribe(ResourceEvent event, std::function<void(std::shared_ptr<Resource>)> callback);
  /**
   * @brief Enables hot-reloading for this resource
   *
   * @param state If true, hot reloading is enabled; if false, it is disabled.
   */
  void setHotReload(bool state);
};

/**
 * @brief Abstract base class for resource loaders.
 *
 * A ResourceLoader is responsible for constructing a specific type of Resource
 * from a file path. Loaders are registered in the ResourceManager by file extension.
 */
class ResourceLoader {
public:
  /**
   * @brief Virtual destructor for proper cleanup of derived loaders.
   */
  virtual ~ResourceLoader() = default;

  /**
   * @brief Loads a resource from a given path.
   *
   * @param path The file path to the resource.
   * @param lazy If true, the resource is created but not loaded until needed.
   * @return A shared pointer to the created resource.
   */
  virtual std::shared_ptr<Resource> load(std::string path, bool lazy) = 0;
};

}
