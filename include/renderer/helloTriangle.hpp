#pragma once

#include <platform/window.hpp>
#include <resource_management/resource_manager.hpp>
#include <core/types.hpp>

#ifdef ENGINE_COMPILE_OPENGL
#include <glad/glad.h>
#endif

using namespace engine::core;
using namespace engine::platform;
using namespace engine::resource_management;

#ifdef ENGINE_COMPILE_OPENGL

void openGLTriangle() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    
    OpenGLWindow window(800, 600, "OpenGL Window");
    window.initWindow();
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    while (!window.shouldClose()) {
        window.clear();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        window.show();
    }

}
#endif

#ifdef ENGINE_COMPILE_VULKAN

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
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

void VulkanTriangle(){
    VulkanWindow window(800, 600, "Vulkan Window");
    window.initWindow();

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
    allocInfo.memoryTypeIndex = findMemoryType( 
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
    while (!window.shouldClose()) {
        glfwPollEvents();
        window.clear();
        window.show();  
    }
}
#endif

#ifdef ENGINE_COMPILE_DIRECTX
void DirectXTriangle(){

    struct vertex {
        float x, y;
    };

    const vertex vertices[] = {
        {0.0f, 0.5f},
        {0.5f, -0.5},
        {-0.5f, -0.5},
    };

    //Buffer desc setup START
    /*this setup a vertex buffer that will only be used by the gpu it holds the vertex data which will be used---
    later on by the pipline is the buffer*/
    D3D11_BUFFER_DESC bufferDesc = {}; //zeros the buffer desc
    bufferDesc.Usage = D3D11_USAGE_DEFAULT; //defualt useage of the buffer means it will live on the GPU
    bufferDesc.ByteWidth = sizeof(vertices); //gets the byte size of the vertices 
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //specfies that this buffer will be used for vertex
    bufferDesc.CPUAccessFlags = 0; //this disallows the CPU from accessing the buffer
    //Buffer desc setup END

    D3D11_SUBRESOURCE_DATA sd = {}; //zeros the subsource data
    sd.pSysMem = vertices; //this will tell dx to copy the vertices into the buffer when the buffer is created later on

    DirectXWindow window(800, 600, "DX Window");
    window.initWindow();

    ID3D11Buffer* vertexBuffer = nullptr;

    DirectXWindow::device->CreateBuffer(&bufferDesc, &sd, &vertexBuffer); //this line inserts the triangle into the GPU through creating a buffer with the rules set by bufferDesc and subsource data in mind

    std::ifstream vertexShaderFile("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/VertexShader.hlsl");
    std::string vertexShaderCode((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());

    ID3DBlob* vertexShaderBlob = nullptr;
    D3DCompile(vertexShaderCode.c_str(), vertexShaderCode.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr);

    
    ID3D11VertexShader* vertexShader = nullptr;
    DirectXWindow::device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);

    std::ifstream pixelShaderFile("C:/Users/Sajed/Desktop/GraphicsEngine/Shaders/PixelShader.hlsl");
    std::string pixelShaderCode((std::istreambuf_iterator<char>(pixelShaderFile)), std::istreambuf_iterator<char>());

    ID3DBlob* pixelShaderBlob = nullptr;
    D3DCompile(pixelShaderCode.c_str(), pixelShaderCode.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr);

    ID3D11PixelShader* pixelShader = nullptr;
    DirectXWindow::device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    DirectXWindow::device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &DirectXWindow::inputLayout);

    vertexShaderBlob->Release();
    pixelShaderBlob->Release();

    DirectXWindow::context->IASetInputLayout(DirectXWindow::inputLayout);

    DirectXWindow::context->VSSetShader(vertexShader, nullptr, 0);
    DirectXWindow::context->PSSetShader(pixelShader, nullptr, 0);


    UINT stride = sizeof(vertex); //this is the size of each individual vertex in bytes (8 bytes) 
    UINT offset = 0; //the starting point of the buffer
    DirectXWindow::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset); //tells dx that when its about to draw it should take 8bytes with starting point of 0 from the vertexBuffer
    DirectXWindow::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //tells the gpu how to interpet the data


    while(!window.shouldClose()){
        window.clear();
        DirectXWindow::context->Draw(3, 0);
        window.show();
    }

}
#endif