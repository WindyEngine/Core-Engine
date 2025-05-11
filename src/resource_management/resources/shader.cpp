#include <ios>
#include <iostream>
#include <json/json.hpp>
#include <filesystem>
#include <fstream>
#include <resource_management/resources/shader.hpp>
#include <resource_management/file.hpp>
#include <string>

using namespace engine::resource_management;

Shader::~Shader() {
  ResourceHandle<Shader> temp(this, [](Shader* ptr){});
  this->unload();
}

std::string Shader::getSourceCode() {
  return this->_sourceCode;
}

bool Shader::load() {
  if (this->_loaded) return false;

  std::cout << "Loading: " << this->_path << std::endl;
  this->_sourceCode = readFile(this->_path);

  this->_loaded = true;
  this->emit(ResourceEvent::Load);
  return true;
}

bool Shader::unload() {
  if (!this->_loaded) return false;
  this->emit(ResourceEvent::Unload);

  std::cout << "Unloading: " << this->_path << std::endl;

  this->_loaded = false;
  return true;
}

void Shader::save() {
  std::ofstream metaFile(std::filesystem::path(this->_path).replace_extension(".meta").string());
  if (!metaFile) return;

  nlohmann::json metadata;
  metadata["type"] = "shader";
  metadata["sourceFile"] = this->_path;

  metaFile << metadata.dump(2);
}

ResourceHandle<Resource> ShaderLoader::load(std::string path, bool lazy) {
  if (!std::filesystem::exists(path + ".meta")) this->create(path);

  std::ifstream metaFile(path + ".meta");
  if (!metaFile) return nullptr;

  nlohmann::json metadata;
  metaFile >> metadata;

  if (metadata["type"] != "shader") return nullptr;

  Shader* rawPointer = new Shader(metadata["sourceFile"]);
  ResourceHandle<Shader> shader = std::shared_ptr<Shader>(rawPointer); 

  if (!lazy) shader->load();

  return shader;
}

void ShaderLoader::create(std::string filePath) {
  std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());

  std::ofstream metaFile(filePath + ".meta", std::ios::trunc);
  if (!metaFile) return;

  nlohmann::json metadata;
  metadata["sourceFile"] = std::filesystem::path(filePath).filename().string() + ".shader";
  metadata["type"] = "shader";

  metaFile << metadata.dump(2);

  std::ofstream sourceFile(filePath + ".shader");
#ifdef TEMPLATES_PATH
  std::ifstream defaultSource(std::string(TEMPLATES_PATH) + "/default.shader");
  sourceFile<< defaultSource.rdbuf();
#endif
}
