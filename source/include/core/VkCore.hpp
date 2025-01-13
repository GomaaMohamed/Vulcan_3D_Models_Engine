#ifndef VK_CORE_H_
#define VK_CORE_H_
// File inclusions
//--------------------------
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <unordered_map>
#include <set>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include "InputTypes.hpp"
#include "PhysicalDevices.hpp"

namespace VkEngine
{
    // Configure the needed layers
    //-----------------------------   
        // Define the needed validation layers
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    
    // Activate or deactivate validation layers
    //-----------------------------
    #define NDEBUG
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    // Configure instance extentions
    //-----------------------------
    // Done

    // Configure device extentions
    //-----------------------------
    const std::vector<const char *> deviceExtensions = {"VK_KHR_swapchain"};

    // Define number of frames(double buffering)
    //-----------------------------
    const int MAX_FRAMES_IN_FLIGHT = 2;

    // Define the application class
    //-------------------------------
    class VkCore
    {
    public:
        // Data types
        //----------------------------
        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities_;
            std::vector<VkSurfaceFormatKHR> formats_;
            std::vector<VkPresentModeKHR> present_modes_;
        };
        // Constructors and destructors
        //-------------------------------
        VkCore(GLFWwindow *_window);
        ~VkCore();
        // Draw methods
        void draw_frame();
        void wait_idle();
        void set_frame_resized();

    private:
        // Class data members
        //---------------------------------------------
        GLFWwindow *window_;
        VkInstance vk_instance_;
        VkDebugUtilsMessengerEXT vk_debug_messenger_;
        //---------------------------------------------
        VkSurfaceKHR vk_surface_;
        PhysicalDevices vk_physical_devices_;
        QueueFamilyIndices vk_queue_family_indices_;
        VkDevice vk_logical_device_;
        VkQueue vk_graphics_queue_;
        VkQueue vk_present_queue_;
        //---------------------------------------------
        VkSwapchainKHR vk_swap_chain_;
        std::vector<VkImage> vk_swap_chain_images_;
        VkFormat vk_swap_chain_image_format_;
        VkPresentModeKHR vk_swap_chain_Present_mode_;
        VkExtent2D vk_swap_chain_extent_;
        std::vector<VkImageView> vk_swap_chain_image_views_;
        std::vector<VkFramebuffer> vk_swap_chain_framebuffers_;
        //----------------------------------------------
        VkRenderPass vk_render_pass_;
        VkPipelineLayout vk_pipeline_layout_;
        VkPipeline vk_graphics_pipeline_;
        //----------------------------------------------
        VkCommandPool vk_command_pool_;
        std::vector<VkCommandBuffer> vk_command_buffers_;
        std::vector<VkSemaphore> vk_image_available_semaphores_;
        std::vector<VkSemaphore> vk_render_finished_semaphores_;
        std::vector<VkFence> vk_inflight_fences_;
        //----------------------------------------------
        VkBuffer vk_vertex_buffer_;
        VkDeviceMemory vk_vertex_buffer_memory_;
        VkBuffer vk_index_buffer_;
        VkDeviceMemory vk_index_buffer_memory_;
        //----------------------------------------------
        VkDescriptorSetLayout vk_descriptor_set_layout_;
        std::vector<VkBuffer> vk_uniform_buffers_;
        std::vector<VkDeviceMemory> vk_uniform_buffers_memory_;
        std::vector<void *> vk_uniform_buffers_mapped_;
        VkDescriptorPool vk_descriptor_pool_;
        std::vector<VkDescriptorSet> vk_descriptor_sets_;
        //-----------------------------------------------
        VkImage vk_texture_image_;
        VkDeviceMemory vk_texture_image_memory_;
        VkImageView vk_texture_image_view_;
        VkSampler vk_texture_sampler_;
        uint32_t vk_mip_levels_;
        //-----------------------------------------------
        VkImage vk_depth_image_;
        VkDeviceMemory vk_depth_image_memory_;
        VkImageView vk_depth_image_view_;
        VkSampleCountFlagBits vk_msaa_samples_;
        VkImage vk_color_image_;
        VkDeviceMemory vk_color_image_memory_;
        VkImageView vk_color_image_view_;
        // Class private methods
        //-----------------------------------------------
        // Composite functions
        void init_vulcan();
        void cleanup_vulcan();
        // Instance functions
        void create_instance();
        bool check_validation_layer_support();
        std::vector<const char *> get_instance_extensions();
        bool check_instance_extension_support(const std::vector<const char *> &_instance_extensions);
        // Debug messenger layer functions
        void setup_debug_messenger();
        void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &_create_info);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT _message_severity, VkDebugUtilsMessageTypeFlagsEXT _message_type, const VkDebugUtilsMessengerCallbackDataEXT *_pcallback_data, void *_puser_data);
        // Surface functions
        void create_surface();
        // Physical device and queue familes functions
        void pick_physical_device();
        // Logical device and queues functions
        void create_logical_device();
        // Swap chain functions
        void create_swap_chain();
        void recreate_swap_chain();
        void cleanup_swap_chain();
        SwapChainSupportDetails query_swap_chain_support();
        VkSurfaceFormatKHR choose_swap_surface_format(VkFormat _format, VkColorSpaceKHR _color_space);
        VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR _present_mode);
        VkExtent2D choose_swap_extent();
        // Image and image view creation functions
        VkImageView create_image_view(VkImage _image, VkFormat _format, VkImageAspectFlags _aspect_flags, uint32_t _mip_levels);
        void create_image(uint32_t _width, uint32_t _height, uint32_t _mip_levels, VkSampleCountFlagBits _num_samples, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkImage &_image, VkDeviceMemory &_image_memory);
        // Swap chain image views
        void create_image_views();
        // Pipeline and render pass functions
        void create_graphics_pipeline();
        void create_render_pass();
        std::vector<char> read_file(const std::string &_filename);
        VkShaderModule create_shader_module(const std::vector<char> &_code);
        // Framebuffer functions
        void create_framebuffers();
        // Command pool and buffers functions
        void create_command_pool();
        void create_command_buffers();
        void record_command_buffer(VkCommandBuffer _command_buffer, uint32_t _image_index);
        void create_sync_objects();
        void destroy_sync_objects();
        // Vertex, index. and uniform buffer functions
        void create_vertex_buffer();
        void create_index_buffer();
        void create_uniform_buffers();
        uint32_t find_memory_type(uint32_t _type_filter, VkMemoryPropertyFlags _properties);
        void create_buffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer &_buffer, VkDeviceMemory &_buffer_memory);
        void copy_buffer(VkBuffer _src_buffer, VkBuffer _dst_buffer, VkDeviceSize _size);
        void create_descriptor_set_layout();
        void update_uniform_buffer(uint32_t _current_image);
        void create_descriptor_pool();
        void create_descriptor_sets();
        // Texture functions
        void create_texture_image();
        VkCommandBuffer begin_single_time_commands();
        void end_single_time_commands(VkCommandBuffer _command_buffer);
        void transition_image_layout(VkImage _image, VkFormat _format, VkImageLayout _old_layout, VkImageLayout _new_layout, uint32_t _mip_levels,VkImageAspectFlags _aspect_flags);
        void copy_buffer_to_image(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height);
        void create_texture_image_view();
        void create_texture_sampler();
        // Dipth functions
        void create_depth_resources();
        VkFormat find_depth_format();
        VkFormat find_supported_format(const std::vector<VkFormat> &_candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features);
        bool has_stencil_component(VkFormat _format);
        // model functions
        void load_model();
        // Mip maps and multisampling functions
        void generate_mipmaps(VkImage _image, VkFormat _image_format, int32_t _tex_width, int32_t _tex_height, uint32_t _mip_levels);
        VkSampleCountFlagBits get_max_usable_sample_count();
        void create_color_resources();
    };
}

#endif