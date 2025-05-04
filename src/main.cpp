#include <resource_management/resource_manager.hpp>


using namespace engine::resource_management;

int main() {
  auto rm = ResourceManager::Get();
  rm->registerLoader(".vert", std::make_unique<ShaderLoader>());
  
  auto shader = std::dynamic_pointer_cast<Shader>(rm->load("/home/Zenjar/Documents/projects/graphics_assets/shader.vert"));

  return 0;
}
