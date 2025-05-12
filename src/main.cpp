#include <resource_management/resource_manager.hpp>
#include <CLI11/CLI11.hpp>
#include <renderer/helloTriangle.hpp>


using namespace engine::resource_management;


#ifdef ENGINE_COMPILE_VULKAN
int main(int argc, char**argv) {
  // CLI Logic
  CLI::App cli;
  argv = cli.ensure_utf8(argv);
  std::string assetPath;
  cli.add_option("-p, --project", assetPath, "Path to Project Folder")->required();
  CLI11_PARSE(cli, argc, argv);

  auto rm = ResourceManager::Get(assetPath + "/assets");
  rm->registerLoader<Shader>(std::make_unique<ShaderLoader>());
  ResourceHandle<Shader> shader = rm->load<Shader>("shaders/myShader");

  std::cout << shader->getSourceCode();

  //VulkanTriangle();
  //DirectXTriangle();
  
  return 0;
}
#endif