/// @file resource.hpp
#pragma once

#include <resource_management/resources/resource.hpp>


namespace engine::resource_management {

class Shader : public Resource {
private:
  std::string _sourceCode;

public:
  using Resource::Resource;
  ~Shader();

  std::string getSourceCode();

  bool load() override;
  bool unload() override;
  void save() override;
};

class ShaderLoader : public ResourceLoader {
public:
  ResourceHandle<Resource> load(std::string path, bool lazy) override;
  void create(std::string path) override;
};

}
