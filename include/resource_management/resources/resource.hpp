/**
 * @file resource.hpp
 * @brief Declares the core classes and interfaces for resource management within the engine.
 *
 * This file contains declarations for the `Resource`, `ResourceLoader`, and `ResourceEvent` classes
 * used for managing, loading, unloading, and hot-reloading various engine resources. It provides
 * facilities for dependency tracking, change propagation, and observer pattern implementation
 * to handle resource-related events.
 */
#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>

#include <filewatch/FileWatch.hpp>


namespace engine::resource_management {

/**
 * @brief Forward declaration of the Resource base class.
 */
class Resource;

/**
 * @brief Type alias for a generic resource handle using `std::shared_ptr`.
 *
 * This alias is used to represent a handle to any resource managed by the engine.
 *
 * @tparam T The type of resource.
 */
template<typename T>
using ResourceHandle = std::shared_ptr<T>;
/**
 * @brief Alias for a handle to a generic (non-templated) resource.
 */
using GenericResourceHandle = ResourceHandle<Resource>;

/**
 * @class ResourceEvent
 * @brief Represents an event associated with a resource's lifecycle.
 *
 * Resource events are used to signal actions such as loading, unloading,
 * or reloading of a resource. These events can be subscribed to by other
 * systems or resources to react accordingly.
 */
class ResourceEvent {
public:
  /**
   * @brief Enumerated values representing the type of resource event.
   */
  enum Value {
    Reload, ///< Resource has been reloaded.
    Load,   ///< Resource has been loaded.
    Unload  ///< Resource has been unloaded.
  };

  /**
   * @brief Constructs a ResourceEvent from a given enumerated value.
   *
   * @param v The specific event type.
   */
  ResourceEvent(Value v);

  /**
   * @brief Returns the current value of the event.
   *
   * @return The `Value` enumeration representing the event type.
   */
  Value value();

  /**
   * @brief Implicit conversion operator to `Value`.
   *
   * Allows a `ResourceEvent` to be used as a `Value` directly.
   *
   * @return The event type.
   */
  operator Value() const;

private:
  Value _value; ///< Internal storage for the event type.
};

/**
 * @class Resource
 * @brief Abstract base class for all loadable and reloadable resources.
 *
 * This class defines the lifecycle and interface for all engine resources. It supports
 * features such as hot reloading, dependency management, change propagation, and
 * event subscription via an observer pattern.
 */
class Resource : public std::enable_shared_from_this<Resource> {
protected:
  /**
   * @brief Map associating resource events with a list of callbacks.
   */
  using ResourceEventCallbackMap = std::unordered_map<
    ResourceEvent::Value,
  std::vector<std::function<void(ResourceEvent, GenericResourceHandle)>>>;

  ResourceEventCallbackMap _subscribers;  ///< Map of event type to subscribed callback functions.
  std::vector<std::weak_ptr<Resource>> _dependencies; ///< List of dependent resources (resources this resource depends on).
  std::vector<std::weak_ptr<Resource>> _dependents;   ///< List of dependent resources (resources that depend on this one).

  std::unique_ptr<filewatch::FileWatch<std::string>> _watcher = nullptr;  ///< File watcher for hot-reload capability.
  std::chrono::steady_clock::time_point _lastReload = std::chrono::steady_clock::now(); ///< Last reload timestamp.
  std::mutex _reloadMutex;  ///< Mutex used to ensure thread-safe reloading.

  std::string _name;        ///< Logical name of the resource.
  bool _loaded = false;     ///< Flag indicating whether the resource is currently loaded.
  std::string _path;        ///< Filesystem path to the resource.
  bool _hot_reload = false; ///< Flag indicating if hot-reload is enabled.

public:
  /**
   * @brief Constructs a new Resource.
   *
   * @param name The unique name of the resource.
   * @param path The file path where the resource is located.
   */
  Resource(std::string name, std::string path);

  /**
   * @brief Virtual destructor.
   */
  virtual ~Resource() = default;

  /**
   * @brief Pure virtual method for loading the resource.
   *
   * Must be implemented by derived classes.
   *
   * @return `true` if loading was successful, `false` otherwise.
   */
  virtual bool load() = 0;

  /**
   * @brief Pure virtual method for unloading the resource.
   *
   * Must be implemented by derived classes.
   *
   * @return `true` if unloading was successful, `false` otherwise.
   */
  virtual bool unload() = 0;

  /**
   * @brief Reloads the resource from disk.
   *
   * Invokes the `unload()` and then `load()` methods. Can be triggered manually or
   * by a file watcher if hot reloading is enabled.
   */
  virtual void reload();

  /**
   * @brief Pure virtual method for saving the resource to disk.
   *
   * Must be implemented by derived classes.
   */
  virtual void save() = 0;

  /**
   * @brief Propagates a change event to all dependent resources.
   *
   * Useful when the current resource changes and dependents must be re-evaluated or reloaded.
   */
  virtual void propagateChange();

  /**
   * @brief Adds a dependency on another resource.
   *
   * @param dep A handle to the resource this resource depends on.
   */
  void addDependency(ResourceHandle<Resource> dep);
  /**
   * @brief Registers another resource as dependent on this one.
   *
   * @param dep A handle to the dependent resource.
   */
  void addDependent(ResourceHandle<Resource> dep);

  /**
   * @brief Subscribes a callback to a specific resource event.
   *
   * @param event The event type to subscribe to.
   * @param callback A function to be invoked when the event occurs.
   */
  void subscribe(ResourceEvent event, std::function<void(ResourceEvent, GenericResourceHandle)> callback);

  /**
   * @brief Emits a resource event and notifies all subscribers.
   *
   * @param event The event to emit.
   */
  void emit(ResourceEvent event);

  /**
   * @brief Casts this resource handle to a specific derived type.
   *
   * @tparam T The target type to cast to.
   * @return A shared pointer to the derived type, or `nullptr` if the cast fails.
   */
  template<typename T>
  ResourceHandle<T> cast() {
    return std::dynamic_pointer_cast<T>(shared_from_this());
  }

  /**
   * @brief Enables or disables hot reloading for this resource.
   *
   * @param state `true` to enable hot reloading, `false` to disable it.
   */
  void setHotReload(bool state);

  /**
   * @brief Checks whether hot reloading is enabled for this resource.
   *
   * @return `true` if hot reloading is enabled, `false` otherwise.
   */
  bool isHotReloading();

  /**
   * @brief Checks whether the resource is currently loaded.
   *
   * @return `true` if the resource is loaded, `false` otherwise.
   */
  bool isLoaded();

  /**
   * @brief Returns the name of the resource.
   *
   * @return A string containing the resource's logical name.
   */
  std::string getName();

  /**
   * @brief Returns the file system path of the resource.
   *
   * @return A string representing the full path to the resource on disk.
   */
  std::string getPath();

  /**
   * @brief Returns the approximate memory footprint of the resource.
   *
   * @return The size in bytes of the resource.
   */
  size_t getSize();
};

/**
 * @class ResourceLoader
 * @brief Interface for loading and creating new resources.
 *
 * Implementations of this interface are responsible for reading resource data
 * from disk and constructing appropriate `Resource` objects.
 */
class ResourceLoader {
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~ResourceLoader() = default;

  /**
   * @brief Loads a resource from disk.
   *
   * @param name The name of the resource.
   * @param path The file path of the resource.
   * @param lazy If `true`, the resource will be initialized without loading data until required.
   * @return A handle to the loaded resource.
   */
  virtual ResourceHandle<Resource> load(std::string path, bool lazy) = 0;

  /**
   * @brief Creates a new resource at the specified path.
   *
   * @param path The path at which to create the new resource.
   */
  virtual void create(std::string path) = 0;
};

} // namespace engine::resource_management
