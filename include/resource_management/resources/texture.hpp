/// @file texture.hpp
#pragma once

#include <resource_management/resources/resource.hpp>


namespace engine::resource_management {

class Texture : public Resource {
public:
  using Resource::Resource;
  ~Texture();

  bool load() override;
  bool unload() override;
  void save() override;
};

class TextureLoader : public ResourceLoader {
public:
  ResourceHandle<Resource> load(std::string name, std::string path, bool lazy) override;
  void create(std::string path) override;
};

}
