#include <iostream>
#include <memory>
#include <resource_management/shader.hpp>
#include <string>

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif

using namespace engine::rendering;
using namespace engine::resource_management;

Shader::Shader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name, bool lazy) :
Resource(ResourceType::Shader, name, lazy) {
  this->vertex_shader = vertex;
  this->fragment_shader = fragment;
}

void Shader::linkDependencies() {
  vertex_shader->dependents.push_back(weak_from_this());
  fragment_shader->dependents.push_back(weak_from_this());
}

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
OpenGLShader::OpenGLShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name, bool lazy) :
Shader(vertex, fragment, name, lazy) {
  if (!lazy) this->load();
}

OpenGLShader::~OpenGLShader() {
  this->unload();
}

void OpenGLShader::load() {
  if (this->loaded) return;

  std::cout << "Loading OpenGLShader" << std::endl;

  vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  std::string vertex_source = vertex_shader->read();
  std::string fragment_source = fragment_shader->read();
  const char* vertex_source_c = vertex_source.c_str();
  const char* fragment_source_c = fragment_source.c_str();

  glShaderSource(vertex_shader_id, 1, &vertex_source_c, NULL);
  glShaderSource(fragment_shader_id, 1, &fragment_source_c, NULL);

  glCompileShader(vertex_shader_id);
  glCompileShader(fragment_shader_id);

  int success;
  char infoLog[512];

  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  shader_program_id = glCreateProgram();
  glAttachShader(shader_program_id, vertex_shader_id);
  glAttachShader(shader_program_id, fragment_shader_id);
  glLinkProgram(shader_program_id);

  glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program_id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  this->loaded = true;
}

void OpenGLShader::unload() {
  if (!this->loaded) return;

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
  glDeleteProgram(shader_program_id);

  std::cout << "Unloading OpenGLShader" << std::endl;

  this->loaded = false;
}

void OpenGLShader::useShader() {
  this->load();
  glUseProgram(shader_program_id);
}
#endif

#ifdef ENGINE_COMPILE_METAL
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>

using namespace engine::resource_management;

MetalShader::MetalShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name, bool lazy) :
Shader(vertex, fragment, name, lazy) {
  if (!lazy) this->load();
}

MetalShader::~MetalShader() {
  this->unload();
}

void MetalShader::load() {
  if (this->loaded) return;

  std::cout << "Loading MetalShader" << std::endl;

  id<MTLDevice> device = MTLCreateSystemDefaultDevice();
  if (!device) {
    std::cout << "ERROR::METAL::NO_DEVICE" << std::endl;
    return;
  }

  std::string vertex_src = vertex_shader->read();
  std::string fragment_src = fragment_shader->read();
  std::string full_src = vertex_src + "\n" + fragment_src;

  NSString* msl = [NSString stringWithUTF8String:full_src.c_str()];
  NSError* error = nil;
  library = [device newLibraryWithSource:msl options:nil error:&error];

  if (!library) {
    std::cout << "ERROR::METAL::LIBRARY_FAILED\n" << error.localizedDescription.UTF8String << std::endl;
    return;
  }

  vertex_function = [library newFunctionWithName:@"vertex_main"];
  fragment_function = [library newFunctionWithName:@"fragment_main"];

  if (!vertex_function || !fragment_function) {
    std::cout << "ERROR::METAL::FUNCTION_NOT_FOUND" << std::endl;
    return;
  }

  MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
  descriptor.vertexFunction = vertex_function;
  descriptor.fragmentFunction = fragment_function;
  descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

  pipeline_state = [device newRenderPipelineStateWithDescriptor:descriptor error:&error];
  if (!pipeline_state) {
    std::cout << "ERROR::METAL::PIPELINE_STATE_FAILED\n" << error.localizedDescription.UTF8String << std::endl;
    return;
  }

  this->loaded = true;
}

void MetalShader::unload() {
  if (!this->loaded) return;

  library = nil;
  vertex_function = nil;
  fragment_function = nil;
  pipeline_state = nil;

  std::cout << "Unloading MetalShader" << std::endl;

  this->loaded = false;
}

void MetalShader::useShader() {
  this->load();
  extern id<MTLRenderCommandEncoder> currentEncoder;
  if (pipeline_state && currentEncoder) {
    [currentEncoder setRenderPipelineState:pipeline_state];
  }
}
#endif

#ifdef ENGINE_COMPILE_DIRECTX
DirectShader::DirectShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name, bool lazy) :
Shader(vertex, fragment, name, lazy) {
  if (!lazy) this->load();
}
DirectShader::~DirectShader() {
  this->unload();
}

void DirectShader::load(){
  if (this->loaded) return;

  ID3DBlob* vertexShaderBlob = nullptr;
  D3DCompile(this->vertex_shader->read().c_str(), strlen(this->vertex_shader->read().c_str()),  nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr);

  ID3D11VertexShader* vertexShader = nullptr;
  DirectXWindow::device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);

  ID3DBlob* pixelShaderBlob = nullptr;
  D3DCompile(this->fragment_shader->read().c_str(), strlen(this->fragment_shader->read().c_str()), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr);

  ID3D11PixelShader* pixelShader = nullptr;
  DirectXWindow::device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);

  D3D11_INPUT_ELEMENT_DESC layout[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  DirectXWindow::device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &this->inputLayout);

  vertexShaderBlob->Release();
  pixelShaderBlob->Release();

  DirectXWindow::context->IASetInputLayout(this->inputLayout);

  DirectXWindow::context->VSSetShader(vertexShader, nullptr, 0);
  DirectXWindow::context->PSSetShader(pixelShader, nullptr, 0);
  
  this->loaded = true;
}

void DirectShader::unload(){
  if(!this->loaded) return;

  this->loaded = false;
}

void DirectShader::useShader(){
  this->load();
}
#endif