/*

#include <platform/window.hpp>
#include <iostream>
#include <vector>

using namespace engine::platform;

#ifdef ENGINE_COMPILE_VULKAN
VkDevice VulkanWindow::device = VK_NULL_HANDLE;
VkExtent2D VulkanWindow::swapChainExtent = {800, 600};
VkPhysicalDevice VulkanWindow::physicalDevice = VK_NULL_HANDLE;
VkCommandBuffer VulkanWindow::commandBuffer = VK_NULL_HANDLE;
VkPipeline VulkanWindow::graphicsPipeline = VK_NULL_HANDLE;
uint32_t VulkanWindow::graphicsQueueFamilyIndex = 0;
VkBuffer VulkanWindow::vertexBuffer = VK_NULL_HANDLE;
VkDeviceMemory VulkanWindow::vertexBufferMemory = VK_NULL_HANDLE; 
VkRenderPass VulkanWindow::renderPass = VK_NULL_HANDLE;
VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
VkRenderPass renderPass = VK_NULL_HANDLE;
VkCommandPool commandPool = VK_NULL_HANDLE;
static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

VulkanWindow::VulkanWindow(int width, int height, const char* name) : _width(width), _height(height), _windowName(name) {}

VulkanWindow::~VulkanWindow(){
    vkDeviceWaitIdle(device);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void VulkanWindow::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(_width, _height, _windowName, nullptr, nullptr);


    //VULKAN INFO STRUCT START
    VkApplicationInfo appInfo{}; //initalizes the Vulkan info variable
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; //initalizes the vulkan struct type
    appInfo.pApplicationName = "Vulkan Window Test"; //the name of the vulkan application
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); //sets up the version of the vulkan app
    appInfo.pEngineName = "Vulkan Engine"; //name of the engine used
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); //version of the engine used
    appInfo.apiVersion = VK_API_VERSION_1_0; //version of the vulkan api used
    //VULKAN INFO STRUCT END

    //VULKAN INFO FOR INSTANCE CREATION START
    VkInstanceCreateInfo createInfo{}; //initalizes the vulkan createInfo var for instance
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;//initializes the vulkan info type (mandatory)
    createInfo.pApplicationInfo = &appInfo; //meta data of the vulkan app info done above

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; //sets what kind of validation layer we will use
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); //sets how many validation layers
    createInfo.ppEnabledLayerNames = validationLayers.data(); //pass the validation layers names to the vulkan instance
    

    uint32_t glfwExtensionCount = 0; //amount of vulkan extension that glfw needs
    const char** glfwExtensions; //pointer to extension names
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); //gets the extension names
    createInfo.enabledExtensionCount = glfwExtensionCount; //set extension amount
    createInfo.ppEnabledExtensionNames = glfwExtensions; //pass names of extension to vulkan
    //VULKAN INFO FOR INSTANCE CREATION END
    
    //the actual creation of the instance think of  an instance as a wire that connects to the vulkan driver which allows you to talk to the vulkan system
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    //this is where the actual linking happens
    //the winodw surface connects the OS window to the vulkan driver by using the instance
    if (glfwCreateWindowSurface(instance, _window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }

    //CHOOSING A SUITBLE GPU START
    uint32_t deviceCount = 0; //a variable to store how many GPUs that are suitble for vulkan
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr); //the actual query to check the GPUs
    std::vector<VkPhysicalDevice> devices(deviceCount); //creation of a dynamic array to hold the GPUs info
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()); //populates the vector with all the GPUs
    physicalDevice = devices[0]; //select the first GPU
    //CHOOSING A SUITBLE GPU END

    //QUEUE FAMILY SETUP START
    //vulkan coomands runs in queues said queues are stored in families (graphics, compute, transfer etc..)
    //not all queues can do everything its a must to query all queueFamilies and find out what are they capable of
    uint32_t queueFamilyCount = 0; //a var to store the queue
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr); //actually querys how many queueFamilies the GPU can handle
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount); //vector to store the queueFamilies properties
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data()); //stores the queueFamilies properites
    //QUEUE FAMILY SETUP END

    //QUEUE FAMILY PROPERTIY CHECK (MOSTLY FOR PRESENTATION AND DRAWING) START
    graphicsQueueFamilyIndex = -1;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { //this first loop looks for a queueFamily that supports graphic commands
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport); //this checks if said queueFamily can also present on a window
            if (presentSupport) {
                graphicsQueueFamilyIndex = i;
                break;
            }
        }
    }
    //QUEUE FAMILY PROPERTIY CHECK (MOSTLY FOR PRESENTATION AND DRAWING) START

    if (graphicsQueueFamilyIndex == -1) {
        throw std::runtime_error("Failed to find a graphics queue family.");
    }

    //PHYSCIAL DEVICE CREATION FOR GRAPHIC QUEUE START
    float queuePriority = 1.0f; //vulkan requires a priority (0.0 to 1.0) for each queue when you create it
    VkDeviceQueueCreateInfo queueCreateInfo = {}; //initlaize queue info
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; //sets the structure type
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex; //this takes the queueFamily found above and tells vulkan to use it to take indvisual queues from said family
    queueCreateInfo.queueCount = 1; //amount of queues you want
    queueCreateInfo.pQueuePriorities = &queuePriority; //the priority of said queue

    VkDeviceCreateInfo deviceCreateInfo = {}; //this info section explains how the device should operate
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; //sets the structure type
    deviceCreateInfo.queueCreateInfoCount = 1; //the amount of queues we are about to pass to the device (only 1 atm)
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; //passing the info of the queue above

    const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; //sets what extension the device will use (here i set it for swapchain)
    deviceCreateInfo.enabledExtensionCount = 1; //amount of extension to enable 
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions; //pointer to the extensions set
    deviceCreateInfo.enabledLayerCount = 0; //disables debugging

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) { //this actually creates the device with all needed info
        throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue); //retrives the queue from the device to use for drawing etc
    //PHYSCIAL DEVICE CREATION FOR GRAPHIC QUEUE END

    //SURFACE FORMAT PICKER START
    uint32_t formatCount; //var to store all available surface formats from the GPU
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr); //gets the number of surface formats
    std::vector<VkSurfaceFormatKHR> formats(formatCount); //storage spaces for the amount of formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data()); //starts filling the storage

    VkSurfaceFormatKHR surfaceFormat = formats[0]; //fall back for just in case

    //iterates over all formats to find with the specifications we need
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = availableFormat;
            break;
        }
    }
    //SURFACE FORMAT PICKER START

    //SWAPCHAIN CREATION START
    VkSwapchainCreateInfoKHR swapchainCreateInfo{}; //info structure for swapchain
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; //set the type
    swapchainCreateInfo.surface = surface; //sets where the swapchain will reneder to
    swapchainCreateInfo.minImageCount = 2; //min amount of images that the swapchain will use
    swapchainCreateInfo.imageFormat = surfaceFormat.format; //the format for the swapchain image 8rgbA
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace; //define the color spaces for the surface (nonlinear rgbA in this case)
    swapchainCreateInfo.imageExtent = { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) }; //set the height and width of the swapchain
    swapchainCreateInfo.imageArrayLayers = 1; //amount of layring the swapchain will use (1 for 2d objects)
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //sets how the image will be used in this case it will be used for color attchment which means it will store the rendered output first
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //how the image will be shared between queues its set to exclusive because only 1 queue is used
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; //sets the transformation of the image before showing (rotate flip etc) this is set to non
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //used for rendering on top of other objects
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; //sets the present mode of the swapchain
    swapchainCreateInfo.clipped = VK_TRUE; //skips rendering of pixels hidden by other windows (good for preformance mostly i think)
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE; //tells it that its a new swapchain no old one is in use

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) { //actually creates the swapchain after setting the info
        throw std::runtime_error("Failed to create swapchain!");
    }
    //SWAPCHAIN CREATION END

    
    //IMAGE VIEW CREATION START
    uint32_t swapChainImageCount = 0; //var to store the amount of swapchain images available
    vkGetSwapchainImagesKHR(device, swapchain, &swapChainImageCount, nullptr); //gets said images from the swapchain and stores it in the var above
    std::vector<VkImage> swapChainImages(swapChainImageCount); //creates a vector to store the images
    vkGetSwapchainImagesKHR(device, swapchain, &swapChainImageCount, swapChainImages.data()); //actually stores said images

    swapChainImageViews.resize(swapChainImageCount); //creates an image view for every image the exists in the swapchain
    for (size_t i = 0; i < swapChainImages.size(); ++i) { //loop through every image and sets information for it to create it
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; //sets the structure of the image view
        viewInfo.image = swapChainImages[i]; //sets which image we are creating a view for
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; //sets the type of the image view (2d)
        viewInfo.format = swapchainCreateInfo.imageFormat; //sets the view format the same as the image formate of the swapchain
        viewInfo.components = {
            VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A
        }; //tells it to use the normal rgba
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //sets what type of data the image will use  
        viewInfo.subresourceRange.baseMipLevel = 0; //no mip
        viewInfo.subresourceRange.levelCount = 1; //amount of levels of mips
        viewInfo.subresourceRange.baseArrayLayer = 0; //amount of layers (its 2d so no need)
        viewInfo.subresourceRange.layerCount = 1; //number of array layers in the view

        if (vkCreateImageView(device, &viewInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) { //actually creates the image view
            throw std::runtime_error("Failed to create image views!");
        }
    }
    //IMAGE VIEW CREATION END
    
    //COMMAND POOL SET UP START
    VkCommandPoolCreateInfo poolInfo{}; //init for the pool info
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; //set the structure for the pool
    poolInfo.queueFamilyIndex = VulkanWindow::graphicsQueueFamilyIndex; //tells the pool what queue family to use
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(VulkanWindow::device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) { //creates the command pool (command pools are used to allocate memory for the command buffer which are the instructions on how to control the gpu)
        throw std::runtime_error("Failed to create command pool!");
    }
    //COMMAND POOL SET UP END

    //COMMAND POOL ALLOCATION START
    VkCommandBufferAllocateInfo allocInfo{}; //init info for allocater
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; //set structure type
    allocInfo.commandPool = commandPool; //tells the allocater which command pool to allocate from
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //buffer level setter
    allocInfo.commandBufferCount = 1; //amount of command buffers

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) { //allocates command buffers from the command pool
        throw std::runtime_error("Failed to allocate command buffer!");
    }
    //COMMAND POOL ALLOCATION END

    //SYNC CREATION START
    //semaphores and fences are used for coordinating the rendering process of the triangle
    VkSemaphoreCreateInfo semaphoreInfo{}; //init for the semaphore info
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; //sets the structure
    semaphoreInfo.pNext = nullptr; //defualt value
    semaphoreInfo.flags = 0; //defualt value

    VkFenceCreateInfo fenceInfo{}; //init for the fence inof
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO; //sets the structure
    fenceInfo.pNext = nullptr; //defualt value
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; //start in a singled state so the first call doesn't block



    //here we are creating 2 sempahores first one signels that the image is available for rendering the second one signels that the rendering is finished and is ready to be presented
    //the fence makes sure that the CPU waits for the GPU to finish using the resources before the CPU uses them
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create synchronization objects for a frame!");
    }
    //SYNC CREATION END

    //RENDER PASS INIT AND CREATION START
    //a render pass is how the rendering will proceeed and how attachments like color are used during the render time
    VkAttachmentDescription colorAttachment = {}; //init the color attachments
    colorAttachment.format = swapchainCreateInfo.imageFormat; //sets the format of the color buffer it has to be the same as the swapchain format
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //multisampling (no multisampling here)
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //clears the image to a pre-defined color before the rendering starts
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //stoore the resulting colors to be used later for rendering
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //disable stencil
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //disable stencil
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //the layout of the content are not preserved
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //after rendering is complete present it through the swapchain

    VkAttachmentReference colorAttachmentRef = {}; //init for color refrences
    colorAttachmentRef.attachment = 0; //pick the first attachement for the subpas
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //use the optimized layout for writing color data

    VkSubpassDescription subpass = {}; //init for a subpass (a subpass is a stage inside of the render pass process a render pass can have multiple and each subpass can use attachments)
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //sets this subpass for graphical pipilines
    subpass.colorAttachmentCount = 1; //the amount of color attachment this subpass wil use
    subpass.pColorAttachments = &colorAttachmentRef; //define which color attachemnt will be used

    VkRenderPassCreateInfo renderPassInfo = {}; //init for the render pass
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; //sets the structeur
    renderPassInfo.attachmentCount = 1; //how many attachemnts this whole rendering process will use
    renderPassInfo.pAttachments = &colorAttachment; //specify which color attachment will be used
    renderPassInfo.subpassCount = 1; //how many subpasses will be used
    renderPassInfo.pSubpasses = &subpass; //specify which subpass wil be used

    //this creates the render pass
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
    //RENDER PASS INIT AND CREATION END

    //FRAMEBUFFER CREATION START
    swapChainFramebuffers.resize(swapChainImageViews.size()); //this will resize the framebuffer storage into how many swapchain images we have (each image in the swapchain will need its own framebuffer because the render renders at 1 image at a time)
    for(size_t i = 0; i < swapChainImageViews.size(); i++){ //for each render view we are creating a buffer for it
        VkImageView attachments[] = { swapChainImageViews[i] }; //sets the attachemnts for the image views

        VkFramebufferCreateInfo framebufferInfo{}; //init for the frame buffer
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO; //sets the structeur of the frame buffer
        framebufferInfo.renderPass = renderPass; //sets which renderpass will be used by this buffer both need to be compatiable with eachother 
        framebufferInfo.attachmentCount = 1; //how many attachments the frame buffer will use
        framebufferInfo.pAttachments = attachments; //the frame buffer will only have one attachment
        framebufferInfo.width = _width; //width of the frame buffer
        framebufferInfo.height = _height; //height of the frame buffer
        framebufferInfo.layers = 1; //only 1 layer is needed for 2d shit

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) { //create the framebuffer
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
    //FRAMEBUFFER CREATION END
}

float VulkanWindow::update(){
    glfwPollEvents();
    return glfwGetTime();
}
void VulkanWindow::clear() {

    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX); //waits for a signal from the fence that all the work that the GPU is doing is now complete so the CPU can start working again
    vkResetFences(device, 1, &inFlightFence); //resets the fence to its unsingaled state

    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex); //acquire the next available image from the swapchain
    vkResetCommandBuffer(commandBuffer, 0); //resets the command buffer to start working on the next render

    VkCommandBufferBeginInfo beginInfo{}; //init for the command buffer info
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; //sets the structure
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) { //starts the recordng of the commands
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    VkClearValue clearColor = { {{1.0f, 0.0f, 0.0f, 1.0f}} }; //sets how the color attachment should be cleared
    
    VkRenderPassBeginInfo renderPassInfo{}; //init for the render pass begin
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; //sets the structuer
    renderPassInfo.renderPass = renderPass; //tells it what render pass we will use
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex]; //sets which frame buffer we will render to
    renderPassInfo.renderArea.offset = {0, 0}; //set where the rendering in the window will start (top-left corner)
    renderPassInfo.renderArea.extent = swapChainExtent; //sets the width and height of the rendering proccess
    renderPassInfo.clearValueCount = 1; //how many clears we have
    renderPassInfo.pClearValues = &clearColor; //the color clearing value we will use right before rendering

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //creates a list of commands that will be used later by the GPU using the info from above

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); //binds the graphic pipline to the rendering proccess
    VkDeviceSize offsets[] = { 0 }; //init an array of offsets which will be used to bind the vertexBuffer (we only have 1 buffer so the offset starts at a 0)
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets); //binds the vertex buffer to the command buffer
    vkCmdDraw(commandBuffer, 3, 1, 0, 0); //issues a draw command

    vkCmdEndRenderPass(commandBuffer); //ends this subpass 

    //this is where the entire process actually works
    //this ends the command buffer meaning that all the commands issued to the commandbuffer will now be executed by the GPU
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void VulkanWindow::show() {
    //this block of code will submit the commandbuffer to the queue
    VkSubmitInfo submitInfo{}; //init for the submit info
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; //set the structure

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore }; //tells the GPU to wait before executing the commands from the commandbuffer
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; //sets where to actually wait (this will wait before the color attachemnt is set to the framebuffer to ensure that the image will be avaialbe for execution)

    submitInfo.waitSemaphoreCount = 1; //how many semaphore to wait for
    submitInfo.pWaitSemaphores = waitSemaphores; //which semaphore to wait on
    submitInfo.pWaitDstStageMask = waitStages; //at what stage to wait

    submitInfo.commandBufferCount = 1; //how many commandbuffers will be used
    submitInfo.pCommandBuffers = &commandBuffer; //which commandbuffer to be used and executed

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore }; //signals to the second semaphore that the rendering is now complete and the draw command has been executed
    submitInfo.signalSemaphoreCount = 1; //how signale will be used
    submitInfo.pSignalSemaphores = signalSemaphores; //which signal will be used

    //this will submit the commandbuffer to the graphicsQueue 
    VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);
    if (result != VK_SUCCESS) {
        std::cerr << "vkQueueSubmit failed: " << result << std::endl;
    }

    //this section will actually present the image from the swapchain
    VkPresentInfoKHR presentInfo{}; //init for the presentaion info
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; //sets a structuer
    presentInfo.waitSemaphoreCount = 1; //how many semaphores to wait on
    presentInfo.pWaitSemaphores = signalSemaphores; //which semaphore to wait on
    presentInfo.swapchainCount = 1; //how many swapchain are used
    presentInfo.pSwapchains = &swapchain; //which swapchain to be used
    presentInfo.pImageIndices = &imageIndex; //this is the image that has the complete rendering and we want to present

    result = vkQueuePresentKHR(graphicsQueue, &presentInfo); //this will insert the presentaion request to the graphiscQueue which then will be executed
    if (result != VK_SUCCESS) {
        std::cerr << "vkQueuePresentKHR failed: " << result << std::endl;
    }
}

bool VulkanWindow::shouldClose(){
    return glfwWindowShouldClose(_window);
}
#endif
*/