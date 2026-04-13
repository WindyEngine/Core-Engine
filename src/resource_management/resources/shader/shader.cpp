#include <ios>

#include <resource_management/resources/shader/shader.hpp>
#include <resource_management/file.hpp>

#include <json/json.hpp>
#include <slang/slang.h>


using namespace engine::resource_management;

const std::string Shader::extension = ".slang";

Shader::~Shader() {
  ResourceHandle<Shader> temp(this, [](Shader* ptr){}); // No-op deleter to avoid double delete
  this->unload();
}

std::string Shader::getSourceCode() {
  this->load(); // Ensure the shader is loaded before accessing the source code
  return this->_sourceCode;
}

std::string Shader::getHLSL(std::string entryPoint) {
  auto compiler = ShaderCompiler::Get();
  return compiler->getString(compiler->Compile(this->cast<Shader>(), entryPoint, SLANG_HLSL, "sm_6_7"));
}

std::string Shader::getGLSL(std::string entryPoint) {
  auto compiler = ShaderCompiler::Get();
  return compiler->getString(compiler->Compile(this->cast<Shader>(), entryPoint, SLANG_GLSL, "glsl_460"));
}

std::string Shader::getMSL(std::string entryPoint) {
  auto compiler = ShaderCompiler::Get();
  return compiler->getString(compiler->Compile(this->cast<Shader>(), entryPoint, SLANG_METAL, "metallib_2_4"));
}

std::vector<uint32_t> Shader::getSPIRV(std::string entryPoint) {
  auto compiler = ShaderCompiler::Get();
  return compiler->getBinary(compiler->Compile(this->cast<Shader>(), entryPoint, SLANG_SPIRV, "spirv_1_6"));
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
  std::ofstream metaFile(std::filesystem::path(this->_path).replace_extension(".meta"));
  if (!metaFile) return;

  nlohmann::json metadata;
  metadata["name"] = this->_name;
  metadata["type"] = "shader";
  metadata["sourceFile"] = std::filesystem::path(this->_path).filename().string();

  metaFile << metadata.dump(2);
}

ResourceHandle<Resource> ShaderLoader::load(std::string path, bool lazy) {
  if (!std::filesystem::exists(path + ".meta")) this->create(path);

  std::ifstream metaFile(path + ".meta");
  if (!metaFile) return nullptr;

  nlohmann::json metadata;
  metaFile >> metadata;

  if (metadata["type"] != "shader") return nullptr;

  Shader* rawPointer = new Shader(metadata["name"], (std::filesystem::path(path).parent_path() / metadata["sourceFile"]).string());
  ResourceHandle<Shader> shader = std::shared_ptr<Shader>(rawPointer); 

  if (!lazy) shader->load();

  return shader;
}

void ShaderLoader::create(std::string filePath) {
  std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());

  std::ofstream metaFile(filePath + ".meta", std::ios::trunc);
  if (!metaFile) return;

  nlohmann::json metadata;
  metadata["name"] = std::filesystem::path(filePath).filename().replace_extension().string();
  metadata["sourceFile"] = std::filesystem::path(filePath).filename().string() + Shader::extension;
  metadata["type"] = "shader";

  metaFile << metadata.dump(2);

  std::ofstream sourceFile(filePath + Shader::extension);
#ifdef TEMPLATES_PATH
  std::ifstream defaultSource(std::string(TEMPLATES_PATH) + "/default" + Shader::extension);
  sourceFile<< defaultSource.rdbuf();
#endif
}
