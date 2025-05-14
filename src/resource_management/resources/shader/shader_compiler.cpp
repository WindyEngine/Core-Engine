#include <cstdint>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <resource_management/resources/shader/shader_compiler.hpp>
#include <resource_management/resources/shader/shader.hpp>
#include <filesystem>
#include <string>
#include <vector>


using namespace engine::resource_management;

std::shared_ptr<ShaderCompiler> ShaderCompiler::_instance = nullptr;
std::mutex ShaderCompiler::_instance_mutex;

ShaderCompiler::ShaderCompiler() {
  slang::createGlobalSession(this->_globalSession.writeRef());
}

ShaderCompiler::~ShaderCompiler() {

}

std::shared_ptr<ShaderCompiler> ShaderCompiler::Get() {
  std::lock_guard<std::mutex> lock(ShaderCompiler::_instance_mutex);
  if (ShaderCompiler::_instance == nullptr) {
    ShaderCompiler::_instance = std::shared_ptr<ShaderCompiler>(new ShaderCompiler);    
  }

  return ShaderCompiler::_instance;
}

void ShaderCompiler::setTargetDesc(SlangCompileTarget target, std::string profile, slang::SessionDesc& session) {
  this->_target = {};
  this->_target.format = target;
  this->_target.profile = this->_globalSession->findProfile(profile.c_str());
  session.targets = &this->_target;
  session.targetCount = 1;
}

void ShaderCompiler::setMacros(slang::SessionDesc& session) {
  session.preprocessorMacros = this->_macros.data();
  session.preprocessorMacroCount = this->_macros.size();
}

void ShaderCompiler::setOptions(std::string searchPath, slang::SessionDesc& session) {
  this->_options = {
    {
      slang::CompilerOptionName::Include,
      {slang::CompilerOptionValueKind::String, 0, 0, searchPath.c_str(), nullptr}
    }
  };
  session.compilerOptionEntries = this->_options.data();
  session.compilerOptionEntryCount = this->_options.size();
}

Slang::ComPtr<slang::IModule> ShaderCompiler::getModule(ResourceHandle<Shader> shader, Slang::ComPtr<slang::ISession> session) {
  Slang::ComPtr<slang::IModule> module;
  Slang::ComPtr<slang::IBlob> diagnosticsBlob;
  module = session->loadModuleFromSourceString(
    shader->getName().c_str(),
    shader->getPath().c_str(),
    shader->getSourceCode().c_str(),
    diagnosticsBlob.writeRef()
  );

  return module;
}

Slang::ComPtr<slang::IEntryPoint> ShaderCompiler::getEntryPoint(std::string entryPoint, Slang::ComPtr<slang::IModule> module) {
  Slang::ComPtr<slang::IEntryPoint> entry;
  module->findEntryPointByName(entryPoint.c_str(), entry.writeRef());
  
  return entry;
}

Slang::ComPtr<slang::IComponentType> ShaderCompiler::getProgram(Slang::ComPtr<slang::IEntryPoint> entryPoint, Slang::ComPtr<slang::IModule> module, Slang::ComPtr<slang::ISession> session) {
  std::array<slang::IComponentType*, 2> componentTypes = {
    module, entryPoint
  };
  Slang::ComPtr<slang::IComponentType> composedProgram;
  {
    Slang::ComPtr<slang::IBlob> diagnosticsBlob;
    SlangResult result = session->createCompositeComponentType(
      componentTypes.data(),
      componentTypes.size(),
      composedProgram.writeRef(),
      diagnosticsBlob.writeRef()
    );
  }

  Slang::ComPtr<slang::IComponentType> linkedProgram;
  {
    Slang::ComPtr<slang::IBlob> diagnosticsBlob;
    SlangResult result = composedProgram->link(
      linkedProgram.writeRef(),
      diagnosticsBlob.writeRef()
    );
  }

  return linkedProgram;
}

Slang::ComPtr<slang::IBlob> ShaderCompiler::Compile(ResourceHandle<Shader> shader, std::string _entryPoint, SlangCompileTarget target, std::string profile) {
  std::filesystem::path filePath(shader->getPath());

  slang::SessionDesc sessionDesc = {};
  this->setTargetDesc(target, profile, sessionDesc);
  this->setMacros(sessionDesc);
  this->setOptions(filePath.parent_path().string(), sessionDesc);

  Slang::ComPtr<slang::ISession> session;
  this->_globalSession->createSession(sessionDesc, session.writeRef());

  Slang::ComPtr<slang::IModule> slangModule = this->getModule(shader, session);
  Slang::ComPtr<slang::IEntryPoint> entryPoint = this->getEntryPoint(_entryPoint, slangModule);
  Slang::ComPtr<slang::IComponentType> linkedProgram = this->getProgram(entryPoint, slangModule, session);

  Slang::ComPtr<slang::IBlob> code;
  {
    Slang::ComPtr<slang::IBlob> diagnosticsBlob;
    SlangResult result = linkedProgram->getTargetCode(
      0, code.writeRef(), diagnosticsBlob.writeRef()
    );
  }

  return code;
}

std::string ShaderCompiler::getString(Slang::ComPtr<slang::IBlob> kernelCode) {
  return std::string(static_cast<const char*>(kernelCode->getBufferPointer()), kernelCode->getBufferSize());
}

std::vector<uint32_t> ShaderCompiler::getBinary(Slang::ComPtr<slang::IBlob> kernelCode) {
  const uint32_t* data = static_cast<const uint32_t*>(kernelCode->getBufferPointer());
  size_t count = kernelCode->getBufferSize() / sizeof(uint32_t);

  return std::vector<uint32_t>(data, data + count);
}
