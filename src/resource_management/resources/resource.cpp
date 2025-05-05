#include "filewatch/FileWatch.hpp"
#include <iostream>
#include <memory>
#include <resource_management/resources/resource.hpp>

using namespace engine::resource_management;

// Constructor: stores the provided file path
Resource::Resource(std::string path) : path(path) {}

// Adds a resource that this one depends on.
// Also registers this resource as a dependent of the other.
void Resource::addDependency(std::shared_ptr<Resource> dep) {
  this->_dependencies.push_back(dep);
  dep->addDependent(shared_from_this());
}

// Adds a dependent resource that should be notified when this changes.
void Resource::addDependent(std::shared_ptr<Resource> dep) {
  this->_dependents.push_back(dep);
}

// Subscribes a callback function to a specific resource event.
void Resource::subscribe(ResourceEvent event, std::function<void(std::shared_ptr<Resource>)> callback) {
  this->_subscribers[event].push_back(callback);
}

// Reloads the resource by unloading and loading it again,
// then notifies all dependents and event subscribers.
void Resource::reload() {
  if(!this->unload()) return;
  if (!this->load()) return;
  this->propagateChange();
}

// Notifies all dependents by calling their reload method,
// and executes all callbacks subscribed to the Change event.
void Resource::propagateChange() {
  // Reload all dependents
  for (auto& dep : this->_dependents) {
    if (auto shared = dep.lock()) {
      shared->reload();
    }
  }

  // Trigger Change event callbacks
  auto& callbacks = this->_subscribers[ResourceEvent::Change];
  for (auto& callback : callbacks) {
    callback(shared_from_this());    
  }
}

// Enables or disables hot reloading for the resource.
void Resource::setHotReload(bool state) {
  // If hot reload is being enabled, and a watcher already exists, do nothing
  if (state) {
    if (this->_watcher != nullptr) return;  // Prevent setting the watcher again if it's already set
    std::cout << "Setting Watcher" << std::endl;

    // Set up a new file watcher to monitor changes to the resource file
    this->_watcher = std::make_unique<filewatch::FileWatch<std::string>>(
      this->path,  // Path of the resource file to monitor
      [this](const std::string& path, const filewatch::Event change_type) {
        // If the file is modified, reload the resource
        if (change_type != filewatch::Event::modified) return;  // Only act on file modification
        this->reload();  // Reload the resource if it's modified
      }
    );

    return;  // Exit after setting the watcher
  }

  // If hot reload is being disabled, reset the watcher
  this->_watcher.reset();  // Remove the watcher and stop monitoring the file
}
