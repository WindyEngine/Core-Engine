#pragma once

#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <vector>


namespace engine::resource_management {

enum class ResourceType {
  Shader, File
};

class Resource : public std::enable_shared_from_this<Resource> {
protected:
  virtual void load() = 0;
  virtual void unload() = 0;

public:
  int resource_id;
  std::string name;
  bool loaded = false;
  bool lazy;
  uint32_t version = 0;
  ResourceType resource_type;
  std::vector<std::weak_ptr<Resource>> dependents;

  Resource(ResourceType type, std::string name = "", bool lazy = true);
  ~Resource() = default;

  virtual void linkDependencies() {};

  void reload();
};

}
