#ifndef PHYSICAL_DEVICES_H_
#define PHYSICAL_DEVICES_H_

// File inclusions
//--------------------------
#define GLFW_INCLUDE_VULKAN
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
#include <GLFW/glfw3.h>
#include <assert.h>

namespace VkEngine
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family_;
        std::optional<uint32_t> present_family_;
    };
    struct PhysicalDevice
    {
        VkPhysicalDevice physical_device_;                           // Physical device handle
        VkPhysicalDeviceProperties device_properties_;               // Physical device properties
        VkPhysicalDeviceFeatures device_features_;                   // Physical device features
        VkPhysicalDeviceMemoryProperties memory_properties_;         // memory properties for this device
        std::vector<VkQueueFamilyProperties> queuefamily_properties_;// Queue family properties for all queue families of this device
        std::vector<VkBool32> queuefamily_supports_present_;         // Queue family support presenting for all queue families of this device
        std::vector<VkSurfaceFormatKHR> surface_formats_;            // Supported surface formats for this device
        VkSurfaceCapabilitiesKHR surface_capabilities_;              // Supported surface capabilities for this device
        std::vector<VkPresentModeKHR> present_modes_;                // Supported present modes of the surface for this device
    };
    class PhysicalDevices
    {
    public:
        PhysicalDevices();
        ~PhysicalDevices();
        void init(const VkInstance &_instance, const VkSurfaceKHR &_surface);
        QueueFamilyIndices select_device(VkQueueFlags _required_queue_type, bool _supports_present, const std::vector<const char *> &_device_extensions);
        PhysicalDevice &get_selected_device() ;
        bool check_device_extension_support(VkPhysicalDevice _device, const std::vector<const char *> &_device_extensions);
    private:
        std::vector<PhysicalDevice> physical_devices_;
        int selected_device_index_;
    };
}


#endif