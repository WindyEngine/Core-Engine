#pragma once

#include <map>
#include <memory>
#include <resource_management/resources/file.hpp>
#include <resource_management/resources/shader.hpp>


namespace engine::resource_management {

class FileStorage {
private:
  static std::map<std::string, std::shared_ptr<File>> _storage;

public:
  std::shared_ptr<File> load(std::string path, std::string name = "", bool hot_reload = false, bool lazy = true);
  std::shared_ptr<File> get(std::string name);
};

class ShaderStorage {
private:
  static std::map<std::string, std::shared_ptr<Shader>> _storage;

public:
  std::shared_ptr<Shader> load(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name = "", bool lazy = true);
  std::shared_ptr<Shader> get(std::string name);
};

}
