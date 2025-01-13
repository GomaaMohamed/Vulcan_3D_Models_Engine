#include "VkCore.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        return attributeDescriptions;
    }
    bool operator==(const Vertex &other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};
namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >>
                    1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
namespace VkEngine
{
    
    // const std::vector<Vertex> vertices = {
    //     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    //     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    //     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    //     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    //     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    //     {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    //     {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    //     {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
    // const std::vector<uint16_t> indices = {
    //     0, 1, 2, 2, 3, 0,
    //     4, 5, 6, 6, 7, 4};
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    // Extention functions that the vulcan loader unloaded by default
    VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (vkCreateDebugUtilsMessenger != nullptr)
        {
            return vkCreateDebugUtilsMessenger(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
    {

        auto vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (vkDestroyDebugUtilsMessenger != nullptr)
        {
            vkDestroyDebugUtilsMessenger(instance, debugMessenger, pAllocator);
        }
    }

    // Initialize static members
    //-------------------------------------------------------------------------------------------
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    // Constructors and class data types
    //--------------------------------------------------------------------------------------------
    VkCore::VkCore(GLFWwindow *_window):window_(_window),vk_msaa_samples_(VK_SAMPLE_COUNT_1_BIT)
    {
        try
        {
            // initialize vulcan
            init_vulcan();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    VkCore::~VkCore()
    {
        cleanup_vulcan();
    }

    // Composite functions
    //--------------------------------------------------------------------------------------------
    void VkCore::init_vulcan()
    {
        create_instance();
        setup_debug_messenger();
        create_surface();
        pick_physical_device();
        create_logical_device();
        create_swap_chain();
        create_image_views();
        create_render_pass();
        create_descriptor_set_layout();
        create_graphics_pipeline();
        create_command_pool();
        create_color_resources();
        create_depth_resources();
        create_framebuffers();
        create_texture_image();
        create_texture_image_view();
        create_texture_sampler();
        load_model();
        create_vertex_buffer();
        create_index_buffer();
        create_uniform_buffers();
        create_descriptor_pool();
        create_descriptor_sets();
        create_command_buffers();
        create_sync_objects();
    }
    void VkCore::cleanup_vulcan()
    {
        destroy_sync_objects();
        vkDestroyCommandPool(vk_logical_device_, vk_command_pool_, nullptr);
        cleanup_swap_chain();
        vkDestroySampler(vk_logical_device_, vk_texture_sampler_, nullptr);
        vkDestroyImageView(vk_logical_device_, vk_texture_image_view_, nullptr);
        vkDestroyImage(vk_logical_device_, vk_texture_image_, nullptr);
        vkFreeMemory(vk_logical_device_, vk_texture_image_memory_, nullptr);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(vk_logical_device_, vk_uniform_buffers_[i], nullptr);
            vkFreeMemory(vk_logical_device_, vk_uniform_buffers_memory_[i], nullptr);
        }
        vkDestroyDescriptorPool(vk_logical_device_, vk_descriptor_pool_, nullptr);
        vkDestroyDescriptorSetLayout(vk_logical_device_, vk_descriptor_set_layout_, nullptr);
        vkDestroyBuffer(vk_logical_device_, vk_index_buffer_, nullptr);
        vkFreeMemory(vk_logical_device_, vk_index_buffer_memory_, nullptr);
        vkDestroyBuffer(vk_logical_device_, vk_vertex_buffer_, nullptr);
        vkFreeMemory(vk_logical_device_, vk_vertex_buffer_memory_, nullptr);
        vkDestroyPipeline(vk_logical_device_, vk_graphics_pipeline_, nullptr);
        vkDestroyPipelineLayout(vk_logical_device_, vk_pipeline_layout_, nullptr);
        vkDestroyRenderPass(vk_logical_device_, vk_render_pass_, nullptr);
        vkDestroyDevice(vk_logical_device_, nullptr);
        if (enableValidationLayers)
        {
            destroy_debug_utils_messenger_ext(vk_instance_, vk_debug_messenger_, nullptr);
        }
        vkDestroySurfaceKHR(vk_instance_, vk_surface_, nullptr);
        vkDestroyInstance(vk_instance_, nullptr);
    }

    // Instance creation functions
    //--------------------------------------------------------------------------------------------
    void VkCore::create_instance()
    {
        std::vector<const char *> instanceExtensions = get_instance_extensions();
        // Check the validation layers and extensions existence
        //-----------------------------
        if (enableValidationLayers && !check_validation_layer_support())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
        // Check if the extentions are supported or not
        if (!check_instance_extension_support(instanceExtensions))
        {
            throw std::runtime_error("Instance extentions layers requested, but not available!");
        }

        // Fill application info struct
        //-----------------------------
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        // Fill instance info struct
        //--------------------------
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        // Fill the global extentions
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
        // Fill the requires layers
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            populate_debug_messenger_create_info(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
        // Create instance
        //----------------
        if (vkCreateInstance(&createInfo, nullptr, &vk_instance_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }
    bool VkCore::check_validation_layer_support()
    {
        // Get the available validation layers
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // Check if the required validation layers are available or not
        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound)
            {
                return false;
            }
        }
        return true;
    }
    std::vector<const char *> VkCore::get_instance_extensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }
    bool VkCore::check_instance_extension_support(const std::vector<const char *> &instanceExtensions)
    {
        // Get the available instance extentions
        uint32_t extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        // Initialize a set containing the required intsance extentions
        std::set<std::string> requiredExtensions(instanceExtensions.begin(), instanceExtensions.end());
        // Loop on all the avilable instance extentions and erase the existed from the set
        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }
        // Check if the set became empty or not
        return requiredExtensions.empty();
    }

    // Debug messanger functions
    //--------------------------------------------------------------------------------------------
    // Function to check if the validation layers is supported
    void VkCore::setup_debug_messenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populate_debug_messenger_create_info(createInfo);

        if (create_debug_utils_messenger_ext(vk_instance_, &createInfo, nullptr, &vk_debug_messenger_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    void VkCore::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debug_callback;
    }
    VKAPI_ATTR VkBool32 VKAPI_CALL VkCore::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    // Surface functions
    //--------------------------------------------------------------------------------------------
    void VkCore::create_surface()
    {
        // Surface creation is platform dependent so we will use glfw
        if (glfwCreateWindowSurface(vk_instance_, window_, nullptr, &vk_surface_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    // Physical device and queue families functions
    //--------------------------------------------------------------------------------------------
    void VkCore::pick_physical_device()
    {
        vk_physical_devices_.init(vk_instance_, vk_surface_);
        vk_queue_family_indices_ = vk_physical_devices_.select_device(VK_QUEUE_GRAPHICS_BIT, true, deviceExtensions);
        vk_msaa_samples_ = get_max_usable_sample_count();
    }

    // Logical devices and queues functions
    //--------------------------------------------------------------------------------------------
    void VkCore::create_logical_device()
    {
        // Fill queue info struct
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            vk_queue_family_indices_.graphics_family_.value(),
            vk_queue_family_indices_.present_family_.value()};
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        // Set physical device features that must be included into the logical device
        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;
        // Set logical device info and create the device and get the queue
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }
        // Create logical device
        if (vkCreateDevice(vk_physical_devices_.get_selected_device().physical_device_, &createInfo, nullptr, &vk_logical_device_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }
        // Get the queues of this device
        vkGetDeviceQueue(vk_logical_device_, vk_queue_family_indices_.graphics_family_.value(), 0, &vk_graphics_queue_);
        vkGetDeviceQueue(vk_logical_device_, vk_queue_family_indices_.present_family_.value(), 0, &vk_present_queue_);
    }

    // Swap chain functions
    //--------------------------------------------------------------------------------------------
    void VkCore::create_swap_chain()
    {
        SwapChainSupportDetails swapChainSupport = query_swap_chain_support();
        VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
        VkPresentModeKHR presentMode = choose_swap_present_mode(VK_PRESENT_MODE_MAILBOX_KHR);
        VkExtent2D extent = choose_swap_extent();
        // Get a suitable image count to attach with the swap chain
        uint32_t imageCount = swapChainSupport.capabilities_.minImageCount + 1;
        if (swapChainSupport.capabilities_.maxImageCount > 0 && imageCount > swapChainSupport.capabilities_.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities_.maxImageCount;
        }
        // Fill swap chain object parameters struct
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = vk_surface_;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        uint32_t queueFamilyIndices[] = {vk_queue_family_indices_.graphics_family_.value(), vk_queue_family_indices_.present_family_.value()};
        if (vk_queue_family_indices_.graphics_family_ != vk_queue_family_indices_.present_family_)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        createInfo.preTransform = swapChainSupport.capabilities_.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        // Create swap chain
        if (vkCreateSwapchainKHR(vk_logical_device_, &createInfo, nullptr, &vk_swap_chain_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }
        // Get swap chain images
        vkGetSwapchainImagesKHR(vk_logical_device_, vk_swap_chain_, &imageCount, nullptr);
        vk_swap_chain_images_.resize(imageCount);
        vkGetSwapchainImagesKHR(vk_logical_device_, vk_swap_chain_, &imageCount, vk_swap_chain_images_.data());
        // Get swap chain parameters
        vk_swap_chain_image_format_ = surfaceFormat.format;
        vk_swap_chain_extent_ = extent;
        vk_swap_chain_Present_mode_ = presentMode;
    }
    void VkCore::recreate_swap_chain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window_, &width, &height);
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(vk_logical_device_);
        cleanup_swap_chain();
        create_swap_chain();
        create_image_views();
        create_color_resources();
        create_depth_resources();
        create_framebuffers();
    }
    void VkCore::cleanup_swap_chain()
    {
        vkDestroyImageView(vk_logical_device_, vk_color_image_view_, nullptr);
        vkDestroyImage(vk_logical_device_, vk_color_image_, nullptr);
        vkFreeMemory(vk_logical_device_, vk_color_image_memory_, nullptr);
        vkDestroyImageView(vk_logical_device_, vk_depth_image_view_, nullptr);
        vkDestroyImage(vk_logical_device_, vk_depth_image_, nullptr);
        vkFreeMemory(vk_logical_device_, vk_depth_image_memory_, nullptr);
        for (auto framebuffer : vk_swap_chain_framebuffers_)
        {
            vkDestroyFramebuffer(vk_logical_device_, framebuffer, nullptr);
        }
        for (auto imageView : vk_swap_chain_image_views_)
        {
            vkDestroyImageView(vk_logical_device_, imageView, nullptr);
        }
        vkDestroySwapchainKHR(vk_logical_device_, vk_swap_chain_, nullptr);
    }
    VkCore::SwapChainSupportDetails VkCore::query_swap_chain_support()
    {
        // Update surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_devices_.get_selected_device().physical_device_, vk_surface_, &(vk_physical_devices_.get_selected_device().surface_capabilities_));
        // Holder to the swap chain charechterestics of specific device
        SwapChainSupportDetails details;
        details.capabilities_ = vk_physical_devices_.get_selected_device().surface_capabilities_;
        details.formats_ = vk_physical_devices_.get_selected_device().surface_formats_;
        details.present_modes_ = vk_physical_devices_.get_selected_device().present_modes_;
        if (details.formats_.empty() && details.present_modes_.empty())
        {
            throw std::runtime_error("This device doesn't support swap chain!");
        }
        return details;
    }
    VkSurfaceFormatKHR VkCore::choose_swap_surface_format(VkFormat _format, VkColorSpaceKHR _color_space)
    {
        for (const auto &availableFormat : vk_physical_devices_.get_selected_device().surface_formats_)
        {
            if (availableFormat.format == _format && availableFormat.colorSpace == _color_space)
            {
                return availableFormat;
            }
        }
        return vk_physical_devices_.get_selected_device().surface_formats_[0];
    }
    VkPresentModeKHR VkCore::choose_swap_present_mode(VkPresentModeKHR _present_mode)
    {
        for (const auto &availablePresentMode : vk_physical_devices_.get_selected_device().present_modes_)
        {
            if (availablePresentMode == _present_mode)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D VkCore::choose_swap_extent()
    {
        if (vk_physical_devices_.get_selected_device().surface_capabilities_.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return vk_physical_devices_.get_selected_device().surface_capabilities_.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window_, &width, &height);
            VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
            actualExtent.width = std::clamp(actualExtent.width, vk_physical_devices_.get_selected_device().surface_capabilities_.minImageExtent.width, vk_physical_devices_.get_selected_device().surface_capabilities_.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, vk_physical_devices_.get_selected_device().surface_capabilities_.minImageExtent.height, vk_physical_devices_.get_selected_device().surface_capabilities_.maxImageExtent.height);
            return actualExtent;
        }
    }
    void VkCore::create_image_views()
    {
        // Create an image view for each image in the swap chain
        vk_swap_chain_image_views_.resize(vk_swap_chain_images_.size());
        for (uint32_t i = 0; i < vk_swap_chain_images_.size(); i++)
        {
            vk_swap_chain_image_views_[i] = create_image_view(vk_swap_chain_images_[i], vk_swap_chain_image_format_, VK_IMAGE_ASPECT_COLOR_BIT,1);
        }
    }
    VkImageView VkCore::create_image_view(VkImage _image, VkFormat _format, VkImageAspectFlags _aspect_flags, uint32_t _mip_levels)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = _format;
        viewInfo.subresourceRange.aspectMask = _aspect_flags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = _mip_levels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        VkImageView imageView;
        if (vkCreateImageView(vk_logical_device_, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image view!");
        }
        return imageView;
    }

    // Graphics pipeline functions
    //--------------------------------------------------------------------------------------------
    void VkCore::create_graphics_pipeline()
    {
        // Read shader byte files
        auto vertShaderCode = read_file(std::string(SRC_DIR)+"/../build/spir_v/vert.spv");
        auto fragShaderCode = read_file(std::string(SRC_DIR)+"/../build/spir_v/frag.spv");
        // Create shaders
        VkShaderModule vertShaderModule = create_shader_module(vertShaderCode);
        VkShaderModule fragShaderModule = create_shader_module(fragShaderCode);
        // Assign shaders to a stage in a pipline
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
        multisampling.minSampleShading = .2f;        // min fraction for sample shading; closer to one is smoother
        multisampling.rasterizationSamples = vk_msaa_samples_;
 
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &vk_descriptor_set_layout_;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(vk_logical_device_, &pipelineLayoutInfo, nullptr, &vk_pipeline_layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {};  // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = vk_pipeline_layout_;
        pipelineInfo.renderPass = vk_render_pass_;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.pDepthStencilState = &depthStencil;

        if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vk_graphics_pipeline_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(vk_logical_device_, fragShaderModule, nullptr);
        vkDestroyShaderModule(vk_logical_device_, vertShaderModule, nullptr);
    }
    void VkCore::create_render_pass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = vk_swap_chain_image_format_;
        colorAttachment.samples = vk_msaa_samples_;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = find_depth_format();
        depthAttachment.samples = vk_msaa_samples_;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = vk_swap_chain_image_format_;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(vk_logical_device_, &renderPassInfo, nullptr, &vk_render_pass_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }
    VkShaderModule VkCore::create_shader_module(const std::vector<char> &code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(vk_logical_device_, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shaderModule;
    }
    std::vector<char> VkCore::read_file(const std::string &_filename)
    {
        std::ifstream file(_filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    // Framebuffer functions
    //-----------------------------------------
    void VkCore::create_framebuffers()
    {
        // Create an framebuffer for each image view in the swap chain and connect it with a render pass
        vk_swap_chain_framebuffers_.resize(vk_swap_chain_image_views_.size());
        for (size_t i = 0; i < vk_swap_chain_image_views_.size(); i++)
        {
            std::array<VkImageView, 3> attachments = {vk_color_image_view_, vk_depth_image_view_, vk_swap_chain_image_views_[i]};
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = vk_render_pass_;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = vk_swap_chain_extent_.width;
            framebufferInfo.height = vk_swap_chain_extent_.height;
            framebufferInfo.layers = 1;
            if (vkCreateFramebuffer(vk_logical_device_, &framebufferInfo, nullptr, &vk_swap_chain_framebuffers_[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    // Command pool functions
    //--------------------------------------------------------------------------------------------
    void VkCore::create_command_pool()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = vk_queue_family_indices_.graphics_family_.value();
        if (vkCreateCommandPool(vk_logical_device_, &poolInfo, nullptr, &vk_command_pool_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }
    void VkCore::create_command_buffers()
    {
        vk_command_buffers_.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vk_command_pool_;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)vk_command_buffers_.size();
        if (vkAllocateCommandBuffers(vk_logical_device_, &allocInfo, vk_command_buffers_.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
    void VkCore::record_command_buffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        // Start recording a command buffer
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // Start a render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vk_render_pass_;
        renderPassInfo.framebuffer = vk_swap_chain_framebuffers_[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vk_swap_chain_extent_;
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline_);

        VkBuffer vertexBuffers[] = {vk_vertex_buffer_};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, vk_index_buffer_, 0, VK_INDEX_TYPE_UINT32);

        // Set the dynamic states
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)vk_swap_chain_extent_.width;
        viewport.height = (float)vk_swap_chain_extent_.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = vk_swap_chain_extent_;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_layout_, 0, 1, &vk_descriptor_sets_[currentFrame], 0, nullptr);
        // Record the draw command
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        // End the render pass
        vkCmdEndRenderPass(commandBuffer);

        // Finish recording
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
    void VkCore::create_sync_objects()
    {
        vk_image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
        vk_render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
        vk_inflight_fences_.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(vk_logical_device_, &semaphoreInfo, nullptr, &vk_image_available_semaphores_[i]) != VK_SUCCESS ||
                vkCreateSemaphore(vk_logical_device_, &semaphoreInfo, nullptr, &vk_render_finished_semaphores_[i]) != VK_SUCCESS ||
                vkCreateFence(vk_logical_device_, &fenceInfo, nullptr, &vk_inflight_fences_[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }
    void VkCore::destroy_sync_objects()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(vk_logical_device_, vk_image_available_semaphores_[i], nullptr);
            vkDestroySemaphore(vk_logical_device_, vk_render_finished_semaphores_[i], nullptr);
            vkDestroyFence(vk_logical_device_, vk_inflight_fences_[i], nullptr);
        }
    }
    void VkCore::draw_frame()
    {
        vkWaitForFences(vk_logical_device_, 1, &vk_inflight_fences_[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vk_logical_device_, vk_swap_chain_, UINT64_MAX, vk_image_available_semaphores_[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swap_chain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(vk_logical_device_, 1, &vk_inflight_fences_[currentFrame]);

        vkResetCommandBuffer(vk_command_buffers_[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        record_command_buffer(vk_command_buffers_[currentFrame], imageIndex);

        update_uniform_buffer(currentFrame);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {vk_image_available_semaphores_[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vk_command_buffers_[currentFrame];

        VkSemaphore signalSemaphores[] = {vk_render_finished_semaphores_[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(vk_graphics_queue_, 1, &submitInfo, vk_inflight_fences_[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {vk_swap_chain_};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(vk_present_queue_, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            framebufferResized = false;
            recreate_swap_chain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    void VkCore::wait_idle()
    {
        vkDeviceWaitIdle(vk_logical_device_);
    }

    // Vertex buffer functions
    //----------------------------------------------------
    void VkCore::create_vertex_buffer()
    {
        // Create a temp staging buffer backed with its memory in the host visible memory region and copy the data to
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void *data;
        vkMapMemory(vk_logical_device_, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(vk_logical_device_, stagingBufferMemory);
        // Create a permanent staging buffer backed with its memory in the device local memory region and copy the data to
        create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_vertex_buffer_, vk_vertex_buffer_memory_);
        copy_buffer(stagingBuffer, vk_vertex_buffer_, bufferSize);
        // Destroy the temp buffer with its backed memory
        vkDestroyBuffer(vk_logical_device_, stagingBuffer, nullptr);
        vkFreeMemory(vk_logical_device_, stagingBufferMemory, nullptr);
    }
    void VkCore::create_index_buffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(vk_logical_device_, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(vk_logical_device_, stagingBufferMemory);

        create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_index_buffer_, vk_index_buffer_memory_);

        copy_buffer(stagingBuffer, vk_index_buffer_, bufferSize);

        vkDestroyBuffer(vk_logical_device_, stagingBuffer, nullptr);
        vkFreeMemory(vk_logical_device_, stagingBufferMemory, nullptr);
    }
    void VkCore::create_uniform_buffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        vk_uniform_buffers_.resize(MAX_FRAMES_IN_FLIGHT);
        vk_uniform_buffers_memory_.resize(MAX_FRAMES_IN_FLIGHT);
        vk_uniform_buffers_mapped_.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            create_buffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vk_uniform_buffers_[i], vk_uniform_buffers_memory_[i]);
            vkMapMemory(vk_logical_device_, vk_uniform_buffers_memory_[i], 0, bufferSize, 0, &vk_uniform_buffers_mapped_[i]);
        }
    }
    uint32_t VkCore::find_memory_type(uint32_t _type_filter, VkMemoryPropertyFlags _properties)
    {
        for (uint32_t i = 0; i < vk_physical_devices_.get_selected_device().memory_properties_.memoryTypeCount; i++)
        {
            if ((_type_filter & (1 << i)) && (vk_physical_devices_.get_selected_device().memory_properties_.memoryTypes[i].propertyFlags & _properties) == _properties)
            {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }
    void VkCore::create_buffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer &_buffer, VkDeviceMemory &_buffer_memory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = _usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(vk_logical_device_, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vk_logical_device_, _buffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, _properties);
        if (vkAllocateMemory(vk_logical_device_, &allocInfo, nullptr, &_buffer_memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        vkBindBufferMemory(vk_logical_device_, _buffer, _buffer_memory, 0);
    }
    void VkCore::copy_buffer(VkBuffer _src_buffer, VkBuffer _dst_buffer, VkDeviceSize _size)
    {
        VkCommandBuffer commandBuffer = begin_single_time_commands();
        VkBufferCopy copyRegion{};
        copyRegion.size = _size;
        vkCmdCopyBuffer(commandBuffer, _src_buffer, _dst_buffer, 1, &copyRegion);
        end_single_time_commands(commandBuffer);
    }
    void VkCore::create_descriptor_set_layout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(vk_logical_device_, &layoutInfo, nullptr, &vk_descriptor_set_layout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    void VkCore::update_uniform_buffer(uint32_t _current_image)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), vk_swap_chain_extent_.width / (float)vk_swap_chain_extent_.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        memcpy(vk_uniform_buffers_mapped_[_current_image], &ubo, sizeof(ubo));
    }
    void VkCore::create_descriptor_pool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        if (vkCreateDescriptorPool(vk_logical_device_, &poolInfo, nullptr, &vk_descriptor_pool_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
    void VkCore::create_descriptor_sets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, vk_descriptor_set_layout_);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = vk_descriptor_pool_;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        vk_descriptor_sets_.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(vk_logical_device_, &allocInfo, vk_descriptor_sets_.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = vk_uniform_buffers_[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = vk_texture_image_view_;
            imageInfo.sampler = vk_texture_sampler_;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = vk_descriptor_sets_[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = vk_descriptor_sets_[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(vk_logical_device_, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    // Texture functions
    //----------------------------------------------------
    void VkCore::create_texture_image()
    {
        // Load the texture photo from the device
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(std::string(std::string(SRC_DIR) + "/textures/viking_room.png").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        vk_mip_levels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }
        // Create a temp staging buffer in the host visible memory region and copy the image to
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void *data;
        vkMapMemory(vk_logical_device_, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(vk_logical_device_, stagingBufferMemory);
        stbi_image_free(pixels);
        // Create an image backed with its memory in the device local memory
        create_image(texWidth, texHeight, vk_mip_levels_, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_texture_image_, vk_texture_image_memory_);
        // Move texture from the temp staging buffer to the permanent image
        transition_image_layout(vk_texture_image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vk_mip_levels_, VK_IMAGE_ASPECT_COLOR_BIT);
        copy_buffer_to_image(stagingBuffer, vk_texture_image_, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        vkDestroyBuffer(vk_logical_device_, stagingBuffer, nullptr);
        vkFreeMemory(vk_logical_device_, stagingBufferMemory, nullptr);
        generate_mipmaps(vk_texture_image_, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, vk_mip_levels_);
        //transition_image_layout(vk_texture_image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vk_mip_levels_);
    }
    void VkCore::create_image(uint32_t _width, uint32_t _height, uint32_t _mip_levels, VkSampleCountFlagBits _num_samples, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkImage &_image, VkDeviceMemory &_image_memory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = _width;
        imageInfo.extent.height = _height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = _mip_levels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = _format;
        imageInfo.tiling = _tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = _usage;
        imageInfo.samples = _num_samples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateImage(vk_logical_device_, &imageInfo, nullptr, &_image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vk_logical_device_, _image, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, _properties);
        if (vkAllocateMemory(vk_logical_device_, &allocInfo, nullptr, &_image_memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }
        vkBindImageMemory(vk_logical_device_, _image, _image_memory, 0);
    }
    VkCommandBuffer VkCore::begin_single_time_commands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vk_command_pool_;
        allocInfo.commandBufferCount = 1;
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vk_logical_device_, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }
    void VkCore::end_single_time_commands(VkCommandBuffer _command_buffer)
    {
        vkEndCommandBuffer(_command_buffer);
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_command_buffer;
        vkQueueSubmit(vk_graphics_queue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vk_graphics_queue_);
        vkFreeCommandBuffers(vk_logical_device_, vk_command_pool_, 1, &_command_buffer);
    }
    void VkCore::transition_image_layout(VkImage _image, VkFormat _format, VkImageLayout _old_layout, VkImageLayout _new_layout, uint32_t _mip_levels,VkImageAspectFlags _aspect_flags)
    {
        VkCommandBuffer commandBuffer = begin_single_time_commands();
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = _old_layout;
        barrier.newLayout = _new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _image;
        barrier.subresourceRange.aspectMask = _aspect_flags;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = _mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
        if (_old_layout == VK_IMAGE_LAYOUT_UNDEFINED && _new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (_old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (_old_layout == VK_IMAGE_LAYOUT_UNDEFINED && _new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }
        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        end_single_time_commands(commandBuffer);
    }
    void VkCore::copy_buffer_to_image(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height)
    {
        VkCommandBuffer commandBuffer = begin_single_time_commands();
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {_width, _height, 1};
        vkCmdCopyBufferToImage(commandBuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        end_single_time_commands(commandBuffer);
    }
    void VkCore::create_texture_image_view()
    {
        vk_texture_image_view_ = create_image_view(vk_texture_image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,vk_mip_levels_);
    }
    void VkCore::create_texture_sampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = vk_physical_devices_.get_selected_device().device_properties_.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f; //static_cast<float>(vk_mip_levels_ / 2)
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        if (vkCreateSampler(vk_logical_device_, &samplerInfo, nullptr, &vk_texture_sampler_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    // Dipth functions
    //----------------------------------------------------
    void VkCore::create_depth_resources()
    {
        VkFormat depthFormat = find_depth_format();
        create_image(vk_swap_chain_extent_.width, vk_swap_chain_extent_.height, 1, vk_msaa_samples_, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_depth_image_, vk_depth_image_memory_);
        vk_depth_image_view_ = create_image_view(vk_depth_image_, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT,1);
        transition_image_layout(vk_depth_image_, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1,VK_IMAGE_ASPECT_DEPTH_BIT);
    }
    VkFormat VkCore::find_depth_format()
    {
        return find_supported_format(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
    VkFormat VkCore::find_supported_format(const std::vector<VkFormat> &_candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features)
    {
        for (VkFormat format : _candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(vk_physical_devices_.get_selected_device().physical_device_, format, &props);
            if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features)
            {
                return format;
            }
            else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }
    bool VkCore::has_stencil_component(VkFormat _format)
    {
        return _format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT;
    }
    void VkCore::set_frame_resized()
    {
        framebufferResized = true;
    }
    void VkCore::load_model()
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, std::string(std::string(SRC_DIR) + "/models/viking_room.obj").c_str()))
        {
            throw std::runtime_error(warn + err);
        }
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.color = {1.0f, 1.0f, 1.0f};
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
    void VkCore::generate_mipmaps(VkImage _image, VkFormat _image_format, int32_t _tex_width, int32_t _tex_height, uint32_t _mip_levels)
    {
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(vk_physical_devices_.get_selected_device().physical_device_, _image_format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = begin_single_time_commands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = _image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = _tex_width;
        int32_t mipHeight = _tex_height;

        for (uint32_t i = 1; i < _mip_levels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                           _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = _mip_levels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        end_single_time_commands(commandBuffer);
    }
    VkSampleCountFlagBits VkCore::get_max_usable_sample_count()
    {
        VkSampleCountFlags counts = vk_physical_devices_.get_selected_device().device_properties_.limits.framebufferColorSampleCounts & vk_physical_devices_.get_selected_device().device_properties_.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }
    void VkCore::create_color_resources()
    {
        VkFormat colorFormat = vk_swap_chain_image_format_;
        create_image(vk_swap_chain_extent_.width, vk_swap_chain_extent_.height, 1, vk_msaa_samples_, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_color_image_, vk_color_image_memory_);
        vk_color_image_view_ = create_image_view(vk_color_image_, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}