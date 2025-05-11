#include <resource_management/resource_manager.hpp>
#include <CLI11/CLI11.hpp>


using namespace engine::resource_management;

int main(int argc, char**argv) {
  // CLI Logic
  CLI::App cli;
  argv = cli.ensure_utf8(argv);
  std::string assetPath;
  cli.add_option("-p, --project", assetPath, "Path to Project Folder")->required();
  CLI11_PARSE(cli, argc, argv);

  auto rm = ResourceManager::Get(assetPath + "/assets");
  rm->registerLoader<Shader>(std::make_unique<ShaderLoader>());
  rm->registerLoader<Texture>(std::make_unique<TextureLoader>());
  ResourceHandle<Shader> shader = rm->load<Shader>("shaders/myShader");
  ResourceHandle<Texture> texture = rm->load<Texture>("textures/myTexture");

  return 0;
}
