/// @file resource.hpp
#pragma once

#include <resource_management/resources/shader/shader_compiler.hpp>


namespace engine::resource_management {

class Shader : public Resource {
private:
  std::string _sourceCode;

public:
  const static std::string extension;

  using Resource::Resource;
  ~Shader();

  std::string getSourceCode();
  std::string getHLSL(std::string entryPoint);
  std::string getGLSL(std::string entryPoint);
  std::string getMSL(std::string entryPoint);
  std::vector<uint32_t> getSPIRV(std::string entryPoint);

  bool load() override;
  bool unload() override;
  void save() override;
};

class ShaderLoader : public ResourceLoader {
public:
  ResourceHandle<Resource> load(std::string name, std::string path, bool lazy) override;
  void create(std::string path) override;
};

}
