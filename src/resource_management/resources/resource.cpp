#include <resource_management/resources/resource.hpp>

using namespace engine::resource_management;

ResourceEvent::ResourceEvent(ResourceEvent::Value v) : _value(v) {}

ResourceEvent::Value ResourceEvent::value() {
  return this->_value;
}

ResourceEvent::operator Value() const {
  return this->_value;
}

Resource::Resource(std::string name, std::string path) : _name(name), _path(path) {}

void Resource::emit(ResourceEvent event) {
  auto& callbacks = this->_subscribers[event];
  for (auto& callback : callbacks) {
    callback(event, shared_from_this());
  }
}

void Resource::addDependency(ResourceHandle<Resource> dep) {
  this->_dependencies.push_back(dep);
  dep->addDependent(shared_from_this());
}

void Resource::addDependent(ResourceHandle<Resource> dep) {
  this->_dependents.push_back(dep);
}

void Resource::subscribe(ResourceEvent event, std::function<void(ResourceEvent, GenericResourceHandle)> callback) {
  this->_subscribers[event].push_back(callback);
}

void Resource::reload() {
  if (!this->_loaded) return;
  if(!this->unload()) return;
  if (!this->load()) return;

  this->propagateChange();

  this->emit(ResourceEvent::Reload);
}

void Resource::propagateChange() {
  for (auto& dep : this->_dependents) {
    if (auto shared = dep.lock()) {
      shared->reload();
    }
  }
}

void Resource::setHotReload(bool state) {
  this->_hot_reload = state;

  if (state) {
    if (this->_watcher) return;

    this->_watcher = std::make_unique<filewatch::FileWatch<std::string>>(
      this->_path,
      [this](const std::string& path, const filewatch::Event change_type) {
        if (change_type != filewatch::Event::modified) return ;

        std::lock_guard<std::mutex> lock(this->_reloadMutex);
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->_lastReload);

        if (duration.count() < 2000) return;
        this->_lastReload = now;

        this->reload();
      }
    );

    return;
  }

  if (this->_watcher) this->_watcher.reset();
}
bool Resource::isHotReloading() {
  return this->_hot_reload;
}

bool Resource::isLoaded() {
  return this->_loaded;
}

std::string Resource::getName() {
  return this->_name;
}

std::string Resource::getPath() {
  return this->_path;
}

size_t Resource::getSize() {
  return sizeof(*this);
}
