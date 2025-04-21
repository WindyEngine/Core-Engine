#pragma once

#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <vector>


namespace engine::resource_management {

// Resource types (Shader, File)
enum class ResourceType {
  Shader, // Shader resource
  File    // File resource
};

// Base class for resources, supports dependency management, reload, and Lazy-Loading
class Resource : public std::enable_shared_from_this<Resource> {
protected:
  virtual void load() = 0;    // Load resource
  virtual void unload() = 0;  // Unload resource

public:
  int resource_id;  // Unique resource ID
  std::string name; // Resource name
  bool loaded = false;  // Flag for loaded state
  bool lazy;  // Lazy-Loading flag
  uint32_t version = 0; // Resource version
  ResourceType resource_type; // Type of resource
  std::vector<std::weak_ptr<Resource>> dependents;  // Dependent resources

  Resource(ResourceType type, std::string name = "", bool lazy = true); // Constructor
  ~Resource() = default;  // Deconstructor

  virtual void linkDependencies() {}; // Link dependent resources (optional)
  void reload();  // Reload the resource
};

}
