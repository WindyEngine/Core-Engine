/**
 * @file shader_compiler.hpp
 * @brief Declares the ShaderCompiler class responsible for compiling shader code using the Slang shading language system.
 *
 * This file defines a singleton class, `ShaderCompiler`, that integrates with the Slang shader language compiler API
 * to compile shader source code into different formats such as HLSL, GLSL, SPIR-V, or MSL.
 * It provides functions to configure compilation options, set macros, and extract binary or string-based output.
 */
#pragma once

#include <cstdint>

#include <resource_management/resources/resource.hpp>

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang-com-helper.h>


namespace engine::resource_management {

class Shader;

/**
 * @class ShaderCompiler
 * @brief Singleton class that compiles shaders using the Slang shading language compiler.
 *
 * The `ShaderCompiler` is a central utility responsible for translating high-level shader source code into
 * intermediate or target-specific binaries. It uses the Slang API to provide cross-platform shader output,
 * supporting multiple shading languages and backends. It manages the lifecycle of the global Slang session,
 * session descriptors, macros, target descriptors, and compilation options.
 *
 * It is used internally by the `Shader` resource class during calls to `getHLSL`, `getSPIRV`, etc.
 */
class ShaderCompiler {
private:
  /**
   * @brief Static shared instance for the singleton ShaderCompiler.
   */
  static std::shared_ptr<ShaderCompiler> _instance;

  /**
   * @brief Mutex used to ensure thread-safe singleton instance creation.
   */
  static std::mutex _instance_mutex;

  /**
   * @brief Slang global session used to manage the lifetime of all Slang compiler state.
   */
  Slang::ComPtr<slang::IGlobalSession> _globalSession;

  /**
   * @brief Session descriptor used to describe compilation parameters to the Slang session.
   */
  slang::SessionDesc _session_desc = {};

  /**
   * @brief Preprocessor macro definitions that are injected into the shader source at compile time.
   *
   * Macros like `"BIAS"` and `"TIME"` can be used to simulate compile-time constants or platform-specific overrides.
   */
  std::vector<slang::PreprocessorMacroDesc> _macros = {
    { "BIAS", "1138" },
    { "TIME", "float" }
  };

  /**
   * @brief Target descriptor that determines the output backend (e.g., HLSL, SPIR-V).
   */
  slang::TargetDesc _target;

  /**
   * @brief Additional compiler options passed to the Slang compiler for customization.
   */
  std::vector<slang::CompilerOptionEntry> _options;

  /**
   * @brief Private constructor for singleton enforcement.
   *
   * Initializes the global Slang session and default compilation configuration.
   */
  ShaderCompiler();

  /**
   * @brief Configures the target descriptor (backend format and shader model profile).
   *
   * @param target The target backend (e.g., SPIR-V, HLSL).
   * @param profile The shader model/profile string (e.g., `"vs_6_0"`).
   * @param session The Slang session descriptor to populate.
   */
  void setTargetDesc(SlangCompileTarget target, std::string profile, slang::SessionDesc& session);

  /**
   * @brief Sets macro definitions into the session descriptor.
   *
   * @param session The session descriptor being modified.
   */
  void setMacros(slang::SessionDesc& session);

  /**
   * @brief Sets compiler options and include search paths into the session descriptor.
   *
   * @param searchPath The directory to search for include files.
   * @param session The session descriptor being modified.
   */
  void setOptions(std::string searchPath, slang::SessionDesc& session);

  /**
   * @brief Loads the shader source into a Slang module.
   *
   * @param shader The shader resource to compile.
   * @param session A Slang session for the compilation.
   * @return A compiled Slang module object.
   */
  Slang::ComPtr<slang::IModule> getModule(ResourceHandle<Shader> shader, Slang::ComPtr<slang::ISession> session);

  /**
   * @brief Retrieves a specific entry point function from a compiled module.
   *
   * @param entryPoint The function name of the entry point.
   * @param module The compiled module object.
   * @return The Slang representation of the entry point.
   */
  Slang::ComPtr<slang::IEntryPoint> getEntryPoint(std::string entryPoint, Slang::ComPtr<slang::IModule> module);

  /**
   * @brief Assembles a complete shader program from a module and an entry point.
   *
   * @param entryPoint The entry point of the shader.
   * @param module The Slang module.
   * @param session The active Slang session.
   * @return A component type representing the compiled shader program.
   */
  Slang::ComPtr<slang::IComponentType> getProgram(Slang::ComPtr<slang::IEntryPoint> entryPoint, Slang::ComPtr<slang::IModule> module, Slang::ComPtr<slang::ISession> session);

public:
  /**
   * @brief Deleted copy constructor to enforce singleton pattern.
   */
  ShaderCompiler(ShaderCompiler& other) = delete;

  /**
   * @brief Destructor that releases all Slang-related resources.
   */
  ~ShaderCompiler();

  /**
   * @brief Retrieves the singleton instance of the ShaderCompiler.
   *
   * Ensures thread-safe lazy instantiation.
   *
   * @return A shared pointer to the global ShaderCompiler instance.
   */
  static std::shared_ptr<ShaderCompiler> Get();

  /**
   * @brief Compiles a shader to the specified target language and profile.
   *
   * @param shader The shader resource to be compiled.
   * @param entryPoint The entry function within the shader code.
   * @param target The Slang backend target (e.g., `SLANG_SPIRV`, `SLANG_HLSL`).
   * @param profile The shader model/profile string.
   * @return A Slang blob containing the compiled result (binary or text).
   */
  Slang::ComPtr<slang::IBlob> Compile(ResourceHandle<Shader> shader, std::string entryPoint, SlangCompileTarget target, std::string profile);

  /**
   * @brief Converts a Slang blob into a human-readable string.
   *
   * Typically used for text-based shader output formats like HLSL or GLSL.
   *
   * @param kernelCode The compiled shader blob.
   * @return A string representation of the compiled shader.
   */
  std::string getString(Slang::ComPtr<slang::IBlob> kernelCode);

  /**
   * @brief Converts a Slang blob into a vector of 32-bit unsigned integers (e.g., SPIR-V binary).
   *
   * @param kernelCode The compiled shader blob.
   * @return A binary array suitable for GPU pipeline creation.
   */
  std::vector<uint32_t> getBinary(Slang::ComPtr<slang::IBlob> kernelCode);

  /**
   * @brief Deleted assignment operator to enforce singleton pattern.
   */
  void operator=(const ShaderCompiler& other) = delete;
};

} // namespace engine::resource_management
