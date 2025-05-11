#include <iostream>
#include <memory>
#include <string>
#include <resource_management/resources/shader.hpp>


#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif

using namespace engine::platform;
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

#ifdef ENGINE_COMPILE_VULKAN
VulkanShader::VulkanShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name, bool lazy) :
Shader(vertex, fragment, name, lazy) {
  if (!lazy) this->load();
}
VulkanShader::~VulkanShader() {
  this->unload();
}

//this creates a Vulkan shaderModule from raw shader code spv as of now 
VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{}; //init of how the shaderModule structure will look like
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; //set the structure
    createInfo.codeSize = code.size(); //the size of the byteCode shader
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); //pointer to the shader code

    VkShaderModule shaderModule; //variable to store the shader module
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) { //creates the shaderModule
        throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
}

//this function reads the spv file and store so that the shaderModule function can use it
std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary); 

    if (!file.is_open()) { //opens a file
        std::cerr << "Failed to open shader file: " << filename << std::endl;
        throw std::runtime_error("Failed to open shader file!");
    }

    size_t fileSize = (size_t)file.tellg(); //checks the file size
    std::vector<char> buffer(fileSize); //creates an appropiate storage space

    file.seekg(0); //resets the file pointer to the beginning 
    file.read(buffer.data(), fileSize); //reads the entire file

    file.close(); //clean up
    return buffer;
}

//this function is used to select the right type of memory to be used to allocate data to
uint32_t VulkanShader::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties; //variable that stores all proprites
    vkGetPhysicalDeviceMemoryProperties(VulkanWindow::physicalDevice, &memProperties); //this will retrive all available memory types and heaps from the GPU

    //loop until we find a suitble memory type to allocate to
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

void VulkanShader::load() {
  if (this->loaded) return;

  struct vertex {
        float x, y;
  };

  const vertex vertices[] = {
    {0.0f, 0.5f},
    {-0.5f, -0.5f},
    {0.5f, -0.5f},
  };

  //BUFFER CREATION START
  //this buffer will be used to store vertex data which will be used later on by the pipeline
  VkBufferCreateInfo bufferInfo{}; //init of the buffer info
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; //sets the structure for the buffer
  bufferInfo.size = sizeof(vertices); //sets the size of the buffer depending on the vertices
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; //what the buffer will be used for (storing vertex data in this case)
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //sets how the buffer will be used by the queues its exclusive now

  if (vkCreateBuffer(VulkanWindow::device, &bufferInfo, nullptr, &VulkanWindow::vertexBuffer) != VK_SUCCESS) { //this creates the buffer
      throw std::runtime_error("Failed to create vertex buffer!");
  }

  //BINDING OF BUFFER MEMORY START
  //this sets the required memory for the vertexBuffer
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(VulkanWindow::device, VulkanWindow::vertexBuffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{}; //init for allocation info
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; //set the structuer
  allocInfo.allocationSize = memRequirements.size; //set the allocation size the same as the memory requirment size
  allocInfo.memoryTypeIndex = VulkanShader::findMemoryType( 
      memRequirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  ); //sets what memory type we need (one that can be written and read from the CPU and another that allows the GPU to see any modifcation done to the data by the CPU)

  //allocates memeory for the vertexBufferMemory
  if (vkAllocateMemory(VulkanWindow::device, &allocInfo, nullptr, &VulkanWindow::vertexBufferMemory) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate vertex buffer memory!");
  }

  //binds the vertexBufferMemory to the vertexBuffer
  vkBindBufferMemory(VulkanWindow::device, VulkanWindow::vertexBuffer, VulkanWindow::vertexBufferMemory, 0);

  void* vData;
  vkMapMemory(VulkanWindow::device, VulkanWindow::vertexBufferMemory, 0, bufferInfo.size, 0, &vData);//maps the allocated data for the CPU to read and write
  memcpy(vData, vertices, (size_t)bufferInfo.size); //actually copies the vertex data into the mapped memory 
  vkUnmapMemory(VulkanWindow::device, VulkanWindow::vertexBufferMemory); //unmaps the memory after the writing is done
  //BINDING OF BUFFER MEMORY END

  //DATA PREPERATION FOR VERTEX INPUT START
  VkVertexInputBindingDescription bindingDescription{}; //init for the vertexBinding
  bindingDescription.binding = 0; //set the binding point at 0
  bindingDescription.stride = sizeof(vertex);  //the stride will be the same size as the vertex
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //the input rate will advance to the next data per vertex

  VkVertexInputAttributeDescription attributeDescription{}; //init for the vertex description attribute
  attributeDescription.binding = 0; //set the binding point at 0
  attributeDescription.location = 0; //this corresponds to the layout in the shade code 
  attributeDescription.format = VK_FORMAT_R32G32_SFLOAT; //sets the format of the vertex depending on the shader code (this is a 32bit one)
  attributeDescription.offset = 0; //start at the beginning of the vertex

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{}; //init for the vertex info
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; //set the structure of the vertex
  vertexInputInfo.vertexBindingDescriptionCount = 1; //how many binding descriptions we will use
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; //which binding descripion to be used
  vertexInputInfo.vertexAttributeDescriptionCount = 1; //how many attribute description we will use 
  vertexInputInfo.pVertexAttributeDescriptions = &attributeDescription; //which attribute description to be used
  //DATA PREPERATION FOR VERTEX INPUT END

  auto vertCode = readFile("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/shader.vert.spv"); //load the complied spv code
  auto fragCode = readFile("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/shader.frag.spv"); //load the compiled spv code

  VkShaderModule vertShaderModule = createShaderModule(VulkanWindow::device, vertCode); //create ShaderModule for vert
  VkShaderModule fragShaderModule = createShaderModule(VulkanWindow::device, fragCode); //create ShaderModule for frag

  //SHADER MODULE STAGING START
  VkPipelineShaderStageCreateInfo vertShaderStageInfo{}; //init for the vert info stage
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; //set the structure
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; //tells vulkan that this is a vertex
  vertShaderStageInfo.module = vertShaderModule; //gives the vertex module
  vertShaderStageInfo.pName = "main"; //the start function name of the vertex code

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{}; //init for the frag info stage
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; //set the structure
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; //tells vulkan that this is a fragment
  fragShaderStageInfo.module = fragShaderModule; //gives the fragment module
  fragShaderStageInfo.pName = "main"; //the start function name of the fragment code
  
  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo }; //stores the info in an array to be used later for the graphics pipeline
  //SHADER MODULE STAGING END

  //VERTEX DATA INTERPETAION START
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{}; //inti for the input assembly
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; //set the structure
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; //specify how the vertex data should be interpeted
  inputAssembly.primitiveRestartEnable = VK_FALSE;
  //VERTEX DATA INTERPETAION END

  //VIEWPORT AND SCISSOR START
  VkViewport viewport{}; //init the view port
  viewport.x = 0.0f; //set the view port at 0x (top-left)
  viewport.y = 0.0f; //set the view port at 0y (top-left)
  viewport.width = (float)VulkanWindow::swapChainExtent.width; //set the width of the viewport using the swapchain width which is the same as the whole window
  viewport.height = (float)VulkanWindow::swapChainExtent.height; //set the height of the viewport using the swapchain height which is the same as the whole window
  viewport.minDepth = 0.0f; //depth of the viewport
  viewport.maxDepth = 1.0f;

  //scissor rectangle is a filter for pixels only pixels in this region will be drawn
  //the scissor here is set for the whole screen meaning no pixel will be clipped
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = VulkanWindow::swapChainExtent;

  //this combines the viewport and the scissor together which then will be thrown to the graphicPipeline
  VkPipelineViewportStateCreateInfo viewportState{}; //init for the state
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; //sets the structure
  viewportState.viewportCount = 1; //how many viewports will be used
  viewportState.pViewports = &viewport; //which viewport to be used
  viewportState.scissorCount = 1; //how many scissors will be used
  viewportState.pScissors = &scissor; //which scissor to be used
  //VIEWPOR AND SCISSOR END

  //RASTERIZER SETUP START
  //rasterizer convertes geometry to pixels
  VkPipelineRasterizationStateCreateInfo rasterizer{}; //init rasterizer
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; //sets the structure
  rasterizer.depthClampEnable = VK_FALSE; //fragments that are clamped instead of discarded (Set to false)
  rasterizer.rasterizerDiscardEnable = VK_FALSE; //discard geometry
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //sets how the polygon will be filled
  rasterizer.lineWidth = 1.0f; //set the thickness of the pixel line
  rasterizer.cullMode = VK_CULL_MODE_NONE; //set how the backface of the shape will be drawn
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //how the shpape will be drawn clockwise or counter
  rasterizer.depthBiasEnable = VK_FALSE; //enables depth mostly for shadow mapping
  //RASTERIZER SETUP END

  //MULTISAMPLING SETUP START
  VkPipelineMultisampleStateCreateInfo multisampling{}; //init for multisampling 
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; //sets the structure
  multisampling.sampleShadingEnable = VK_FALSE; //disable per sample shading
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //disable multisampling
  //MULTISAMPLING SETUP END

  //COLOR BLENDING SETUP START
  VkPipelineColorBlendAttachmentState colorBlendAttachment{}; //init for blendingAttachment
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //set the mask for rgba
  colorBlendAttachment.blendEnable = VK_FALSE; //no color blending the shape directly replaces the pixels

  VkPipelineColorBlendStateCreateInfo colorBlending{}; //init for colorBlending
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; //sets the structure
  colorBlending.logicOpEnable = VK_FALSE; //no logical opertaion for color blending
  colorBlending.attachmentCount = 1; //how many attchment will be used
  colorBlending.pAttachments = &colorBlendAttachment; //which attachments to be used
  //COLOR BLENDING SETUP END

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {}; //init for th pipeline layout
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; //sets the structure
  pipelineLayoutInfo.setLayoutCount = 0; //we arent using a descriptor set so its set to 0
  pipelineLayoutInfo.pSetLayouts = nullptr; // no descriptor set

  //creates the layout of the pipline which will be used by the graphicPipeline later
  VkPipelineLayout pipelineLayout;
  if (vkCreatePipelineLayout(VulkanWindow::device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout!");
  }

  //GRAPHIC PIPLINE CREATION START
  VkGraphicsPipelineCreateInfo pipelineInfo{}; //init info for the pipeline
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; //sets the structure
  pipelineInfo.stageCount = 2; //how many stages we will use
  pipelineInfo.pStages = shaderStages; //points to both the vertex and fragment shader
  pipelineInfo.pVertexInputState = &vertexInputInfo; //set how the vertex data is read
  pipelineInfo.pInputAssemblyState = &inputAssembly; //set how the vertex data is interpeted
  pipelineInfo.pViewportState = &viewportState; //set the region the framebuffer to draw on
  pipelineInfo.pRasterizationState = &rasterizer; //tells vulkan how to rasterize geometry
  pipelineInfo.pMultisampleState = &multisampling; //config for multipsampling (disabled)
  pipelineInfo.pColorBlendState = &colorBlending; //how to output the colors to the framebuffer (blending is disabled so fragment data will be put directly)
  pipelineInfo.layout = pipelineLayout; //set the pipeline layout
  pipelineInfo.renderPass = VulkanWindow::renderPass; //set what renderpass this pipeline will be used with
  pipelineInfo.subpass = 0; //which subpass in the render pass will be used

  //creates the pipeline
  if (vkCreateGraphicsPipelines(VulkanWindow::device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &VulkanWindow::graphicsPipeline) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline!");
  }
  //GRAPHIC PIPELINE CREATION END

  this->loaded = true;
}

void VulkanShader::unload() {
  if (!this->loaded) return;

  this->loaded = false;
}

void VulkanShader::useShader(){
  this->load();
}
#endif

#ifdef ENGINE_COMPILE_METAL
MetalShader::MetalShader(std::shared_ptr<File> vertex, std::shared_ptr<File> fragment, std::string name, bool lazy) :
Shader(vertex, fragment, name, lazy) {
  if (!lazy) this->load();
}

MetalShader::~MetalShader() {
  this->unload();
}

void MetalShader::load() {
  if (this->loaded) return;

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

  //Shader compilation and creation START
  ID3DBlob* vertexShaderBlob = nullptr; //create a pointer for the shader.hlsl file which will be used to create the shader 
  D3DCompile(this->vertex_shader->read().c_str(), strlen(this->vertex_shader->read().c_str()),  nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr); //this will actually compile the shader and turns into byte code which the shader blob will point to

  ID3D11VertexShader* vertexShader = nullptr; //creates a pointer for the actual shader after compilation
  DirectXWindow::device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader); //creates the shader by getting the buffer location and size in the memory and output it to the vertexShader for the GPU to render
  //Shader compilation and creation END

  //pixel(fragment) compilation and creation START
  ID3DBlob* pixelShaderBlob = nullptr; //create a pointer for the pixel.hlsl file which will be used to create the pixels 
  D3DCompile(this->fragment_shader->read().c_str(), strlen(this->fragment_shader->read().c_str()), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr); //this will actually compile the shader and turns into byte code which the shader blob will point to

  ID3D11PixelShader* pixelShader = nullptr; //creates a pointer for the actual pixel after compilation
  DirectXWindow::device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader); //this will actually compile the shader and turns into byte code which the shader blob will point to
  //pixel(fragment) compilation and creation END

  //LAYOUT CRAETION START
  //the layout is basically how the GPU should interpet the buffer based on rules that you set
  D3D11_INPUT_ELEMENT_DESC layout[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
  }; //this defines how the shader input should be interpeted 
  

  DirectXWindow::device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &this->inputLayout); //this actually creates the layout which tells the GPU how to interpet th vertex buffer based on the shaders
  //LAYOUT CREATION END

  vertexShaderBlob->Release(); //gets rid of the blob because its not needed anymore
  pixelShaderBlob->Release(); //gets rid of the blob because its not needed anymore

  DirectXWindow::context->IASetInputLayout(this->inputLayout); //sets the layout which is needed before the draw function is called

  DirectXWindow::context->VSSetShader(vertexShader, nullptr, 0); //sets the vertex shader in the render pipeline
  DirectXWindow::context->PSSetShader(pixelShader, nullptr, 0); //sets the pixel shader
  
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
