#pragma once

#include "core/types.hpp"
#include <memory>
#include <resource_management/resource_storage.hpp>


namespace engine::resource_management {

class ResourceManagerConfig {
public:
  bool hot_reload = false;
  core::GraphicsAPI api = core::GraphicsAPI::OpenGL;
};

class ResourceManager {
private:
  static FileStorage _file_storage;
  static ShaderStorage _shader_storage;

public:
  static ResourceManagerConfig config;

  static void setGraphicsLoader(core::GraphicsAPI api);
  static void cleanUp();

  static std::shared_ptr<File> getFile(std::string name);
  static std::shared_ptr<Shader> getShader(std::string name);

  static std::shared_ptr<File> loadFile(std::string path, std::string name = "", bool lazy = true);
  static std::shared_ptr<Shader> loadShader(std::string vertex_path, std::string fragment_path, std::string name = "", bool lazy = true);
};

}
