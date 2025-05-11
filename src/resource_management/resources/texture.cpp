#include "resource_management/resources/resource.hpp"
#include "json/json.hpp"
#include <filesystem>
#include <fstream>
#include <resource_management/resources/texture.hpp>


using namespace engine::resource_management;

Texture::~Texture() {
  ResourceHandle<Texture> temp(this, [](Texture* ptr){});
  this->unload();
}

bool Texture::load() {
  if (this->_loaded) return false;

  std::cout << "Loading: " << this->_path << std::endl;
  
  this->_loaded = true;
  this->emit(ResourceEvent::Load);
  return true;
}

bool Texture::unload() {
  if (!this->_loaded) return false;
  this->emit(ResourceEvent::Unload);

  std::cout << "Unloading: " << this->_path << std::endl;

  this->_loaded = false;
  return true;
}

void Texture::save() {
  std::ofstream metaFile(std::filesystem::path(this->_path).replace_extension(".meta"));
  if (!metaFile) return;

  nlohmann::json metadata;
  metadata["type"] = "texture";
  metadata["sourceFile"] = std::filesystem::path(this->_path).filename().string();

  metaFile << metadata.dump(2);
}

ResourceHandle<Resource> TextureLoader::load(std::string path, bool lazy) {
  std::cout << "Test" << std::endl;
  if  (!std::filesystem::exists(path + ".meta")) this->create(path);
  
  std::ifstream metaFile(path + ".meta");
  if (!metaFile) return nullptr;

  nlohmann::json metadata;
  metaFile >> metadata;

  if (metadata["type"] != "texture") return nullptr;

  Texture* rawPointer = new Texture((std::filesystem::path(path).parent_path() / metadata["sourceFile"]).string());
  ResourceHandle<Texture> texture = std::shared_ptr<Texture>(rawPointer);

  if (!lazy) texture->load();

  return texture;
}

void TextureLoader::create(std::string filePath) {
  std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());


  std::ofstream metaFile(filePath + ".meta", std::ios::trunc);
  if (!metaFile) return;

  nlohmann::json metadata;
  metadata["sourceFile"] = std::filesystem::path(filePath).filename().string() + ".png";
  metadata["type"] = "texture";

  metaFile << metadata.dump(2);

  std::ofstream sourceFile(filePath + ".png");
#ifdef TEMPLATES_PATH
  std::ifstream defaultSource(std::string(TEMPLATES_PATH) + "/default.png", std::ios::binary);
  sourceFile<< defaultSource.rdbuf();
#endif
}
