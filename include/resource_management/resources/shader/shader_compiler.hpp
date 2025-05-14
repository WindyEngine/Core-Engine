#pragma once

#include <cstdint>

#include <resource_management/resources/resource.hpp>

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang-com-helper.h>


namespace engine::resource_management {

class Shader;

class ShaderCompiler {
private:
  static std::shared_ptr<ShaderCompiler> _instance;
  static std::mutex _instance_mutex;

  Slang::ComPtr<slang::IGlobalSession> _globalSession;
  slang::SessionDesc _session_desc = {};

  std::vector<slang::PreprocessorMacroDesc> _macros = {
    { "BIAS", "1138" },
    { "TIME", "float" }
  };
  slang::TargetDesc _target;
  std::vector<slang::CompilerOptionEntry> _options;

  ShaderCompiler();

  void setTargetDesc(SlangCompileTarget target, std::string profile, slang::SessionDesc& session);
  void setMacros(slang::SessionDesc& session);
  void setOptions(std::string searchPath, slang::SessionDesc& session);
  Slang::ComPtr<slang::IModule> getModule(ResourceHandle<Shader> shader, Slang::ComPtr<slang::ISession> session);
  Slang::ComPtr<slang::IEntryPoint> getEntryPoint(std::string entryPoint, Slang::ComPtr<slang::IModule> module);
  Slang::ComPtr<slang::IComponentType> getProgram(Slang::ComPtr<slang::IEntryPoint> entryPoint, Slang::ComPtr<slang::IModule> module, Slang::ComPtr<slang::ISession> session);

public:
  ShaderCompiler(ShaderCompiler& other) = delete;
  ~ShaderCompiler();

  static std::shared_ptr<ShaderCompiler> Get();

  Slang::ComPtr<slang::IBlob> Compile(ResourceHandle<Shader> shader, std::string entryPoint, SlangCompileTarget target, std::string profile);
  std::string getString(Slang::ComPtr<slang::IBlob> kernelCode);
  std::vector<uint32_t> getBinary(Slang::ComPtr<slang::IBlob> kernelCode);

  void operator=(const ShaderCompiler& other) = delete;
};

}
