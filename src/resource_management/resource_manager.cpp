#include <memory>
#include <resource_management/factory.hpp>
#include <resource_management/resource_manager.hpp>


using namespace engine::resource_management;

ResourceManagerConfig ResourceManager::config = ResourceManagerConfig();

FileStorage ResourceManager::_file_storage = FileStorage();
ShaderStorage ResourceManager::_shader_storage = ShaderStorage();

void ResourceManager::setGraphicsLoader(core::GraphicsAPI api) {
  FactoryManager::setGraphicsAPI(api);
}
void ResourceManager::cleanUp() {
  _file_storage.clean();
  _shader_storage.clean();
}

std::shared_ptr<File> ResourceManager::getFile(std::string name) {
  return _file_storage.get(name);
}
std::shared_ptr<Shader> ResourceManager::getShader(std::string name) {
  return _shader_storage.get(name);
}

std::shared_ptr<File> ResourceManager::loadFile(std::string path, std::string name, bool lazy) {
  return _file_storage.load(path, name, ResourceManager::config.hot_reload, lazy);
}
std::shared_ptr<Shader> ResourceManager::loadShader(std::string vertex_path, std::string fragment_path, std::string name, bool lazy) {
  std::shared_ptr<File> vertex_file = ResourceManager::loadFile(vertex_path, "", lazy);
  std::shared_ptr<File> fragment_file = ResourceManager::loadFile(fragment_path, "", lazy);

  return _shader_storage.load(vertex_file, fragment_file, name, lazy);
}
