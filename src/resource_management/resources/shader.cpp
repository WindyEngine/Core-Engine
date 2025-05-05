#include <iostream>
#include <resource_management/resources/shader.hpp>

using namespace engine::resource_management;

// Loads the shader from file and marks it as loaded
bool Shader::load() {
  std::cout << "Loading Shader: " << this->path << std::endl;
  this->loaded = true;

  return true;
}

// Unloads the shader and marks it as not loaded
bool Shader::unload() {
  std::cout << "Unloading Shader: " << this->path << std::endl;
  this->loaded = false;

  return true;
}

// Creates a new Shader resource, optionally loading it immediately
std::shared_ptr<Resource> ShaderLoader::load(std::string path, bool lazy) {
  // Create a raw Shader pointer
  Shader* raw = new Shader(path);

  // Wrap the raw pointer in a shared_ptr with a custom deleter
  // that ensures the shader is properly unloaded before deletion
  std::shared_ptr<Shader> shader(raw, [](Shader* ptr) {
    if (!ptr || !ptr->loaded) return;

    ptr->unload();
    delete ptr;
  });

  // If not lazy, load the shader immediately
  if (!lazy) shader->load();

  return shader;
}
