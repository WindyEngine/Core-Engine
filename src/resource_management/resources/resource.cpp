#include <chrono>
#include <resource_management/resources/resource.hpp>


using namespace engine::resource_management;

Resource::Resource(ResourceType type, std::string name, bool lazy) :
resource_type(type), lazy(lazy) {
  this->name = name.empty() ? std::to_string(
    std::hash<std::string>{}(
      std::to_string(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
      )
    )
  ) : name;
}

void Resource::reload() {
  if (!this->loaded) return;
  this->unload();
  this->load();

  this->version++;

  for (std::weak_ptr<Resource>& resource : dependents) {
    if (auto dep = resource.lock()) {
      dep->reload();
    }
  }
}
