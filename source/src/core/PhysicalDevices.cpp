#include "PhysicalDevices.hpp"

namespace VkEngine
{
    // Auxilary local functions
    //-------------------------------------
    static void printImageUsageFlags(const VkImageUsageFlags &flags)
    {
        if (flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        {
            std::cout << "    Image usage transfer src is supported" << std::endl;
        }
        if (flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        {
            std::cout << "    Image usage transfer dest is supported" << std::endl;
        }
        if (flags & VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            std::cout << "    Image usage sampled is supported" << std::endl;
        }
        if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            std::cout << "    Image usage color attachment is supported" << std::endl;
        }
        if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            std::cout << "    Image usage depth stencil attachment is supported" << std::endl;
        }
        if (flags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
        {
            std::cout << "    Image usage transient attachment is supported" << std::endl;
        }
        if (flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
        {
            std::cout << "    Image usage input attachment is supported" << std::endl;
        }
    }
    static void printMemoryProperty(VkMemoryPropertyFlags PropertyFlags)
    {
        if (PropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            std::cout << "Device Local, ";
        }
        if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            std::cout << "Host Visible, ";
        }
        if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        {
            std::cout << "Host Coherent, ";
        }
        if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
        {
            std::cout << "Host Cached, ";
        }
        if (PropertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
        {
            std::cout << "Lazily Allocated, ";
        }
        if (PropertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
        {
            std::cout << "Protected, ";
        }
    }

    // Class implementations
    //-------------------------------------
    PhysicalDevices::PhysicalDevices():selected_device_index_(-1){}
    PhysicalDevices::~PhysicalDevices(){}
    void PhysicalDevices::init(const VkInstance &_instance, const VkSurfaceKHR &_surface)
    {
        // Get the number of the available physical devices
        uint32_t numDevices = 0;
        vkEnumeratePhysicalDevices(_instance, &numDevices, NULL);
        std::cout << std::endl << "Number of physical devices for this host: " << numDevices << std::endl;
        physical_devices_.resize(numDevices);
        // Create temp vector to hold the available physical devices handles
        std::vector<VkPhysicalDevice> Devices(numDevices);
        vkEnumeratePhysicalDevices(_instance, &numDevices, Devices.data());

        // Loop on all the device handles and use it to fill all devices with its all info
        for (uint32_t i = 0; i < numDevices; i++)
        {
            // Start of device
            std::cout << "--------------------------------------------------" << std::endl;
            
            // Fill device handler
            physical_devices_[i].physical_device_ =  Devices[i];

            // Fill device properties and print some of them
            vkGetPhysicalDeviceProperties( Devices[i], &physical_devices_[i].device_properties_);
            std::cout << "Device name: " << physical_devices_[i].device_properties_.deviceName << std::endl;
            uint32_t apiVer = physical_devices_[i].device_properties_.apiVersion;
            std::cout << "    API version: "
                      << VK_API_VERSION_VARIANT(apiVer)
                      << "." << VK_API_VERSION_MAJOR(apiVer)
                      << "." << VK_API_VERSION_MINOR(apiVer) 
                      << "." << VK_API_VERSION_PATCH(apiVer) << std::endl;
            
            // Fill device features
            vkGetPhysicalDeviceFeatures(Devices[i], &physical_devices_[i].device_features_);

            // Get number of queue families attached with the physical device
            uint32_t numQFamilies = 0;
            vkGetPhysicalDeviceQueueFamilyProperties( Devices[i], &numQFamilies, NULL);
            std::cout << "    Number of queue families for this device: " << numQFamilies << std::endl;
            physical_devices_[i].queuefamily_properties_.resize(numQFamilies);
            physical_devices_[i].queuefamily_supports_present_.resize(numQFamilies);
            // Fill queue families properties for this device
            vkGetPhysicalDeviceQueueFamilyProperties(Devices[i], &numQFamilies, physical_devices_[i].queuefamily_properties_.data());
            // Loop on all queue families and fill their support present state
            for (uint32_t q = 0; q < numQFamilies; q++)
            {
                const VkQueueFamilyProperties &QFamilyProp = physical_devices_[i].queuefamily_properties_[q];
                std::cout << "    Family " << q << " has " << QFamilyProp.queueCount << " queues";
                VkQueueFlags Flags = QFamilyProp.queueFlags;
                std::cout << " >>>> GFX "
                          << ((Flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No")
                          << ", Compute " 
                          << ((Flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No")
                          << ", Transfer "
                          << ((Flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No")
                          << ", Sparse binding "
                          << ((Flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No")
                          << std::endl;
                vkGetPhysicalDeviceSurfaceSupportKHR( Devices[i], q, _surface, &(physical_devices_[i].queuefamily_supports_present_[q]));
            }

            // Get number of surface formats for this device and fill them
            uint32_t numFormats = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR( Devices[i], _surface, &numFormats, NULL);
            physical_devices_[i].surface_formats_.resize(numFormats);
            vkGetPhysicalDeviceSurfaceFormatsKHR( Devices[i], _surface, &numFormats, physical_devices_[i].surface_formats_.data());
            std::cout << "    Number of surface formats for this device: " << numFormats << std::endl;
            for (uint32_t j = 0; j < numFormats; j++)
            {
                const VkSurfaceFormatKHR &SurfaceFormat = physical_devices_[i].surface_formats_[j];
                std::cout << "    Format "
                          << SurfaceFormat.format
                          << " color space "
                          << SurfaceFormat.colorSpace
                          << std::endl;
            }

            std::cout << "    Surface capabilities for this device: " << std::endl;
            // Fill device surface capabilities
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( Devices[i], _surface, &(physical_devices_[i].surface_capabilities_));
            printImageUsageFlags(physical_devices_[i].surface_capabilities_.supportedUsageFlags);

            // Get number of present modes of a surface and fill them
            uint32_t numPresentModes = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR( Devices[i], _surface, &numPresentModes, NULL);
            physical_devices_[i].present_modes_.resize(numPresentModes);
            vkGetPhysicalDeviceSurfacePresentModesKHR( Devices[i], _surface, &numPresentModes, physical_devices_[i].present_modes_.data());

            // Fill device memory properties
            vkGetPhysicalDeviceMemoryProperties( Devices[i], &(physical_devices_[i].memory_properties_));
            std::cout << "    Number of memory types for this device: "
                      << physical_devices_[i].memory_properties_.memoryTypeCount
                      << std::endl;
            for (uint32_t j = 0; j < physical_devices_[i].memory_properties_.memoryTypeCount; j++)
            {
                std::cout << "    "
                          << j
                          << ": flags "
                          << physical_devices_[i].memory_properties_.memoryTypes[j].propertyFlags
                          << " heap "
                          << physical_devices_[i].memory_properties_.memoryTypes[j].heapIndex
                          << " ";
                printMemoryProperty(physical_devices_[i].memory_properties_.memoryTypes[j].propertyFlags);
                std::cout << std::endl;
            }
            std::cout << "    Number of heap types: " 
            << physical_devices_[i].memory_properties_.memoryHeapCount
            << std::endl;

        }
    }
    QueueFamilyIndices PhysicalDevices::select_device(VkQueueFlags _required_queue_type, bool _supports_present, const std::vector<const char *> &_device_extensions)
    {
        QueueFamilyIndices indices;
        selected_device_index_ = -1;
        for (uint32_t i = 0; i < physical_devices_.size(); i++)
        {
            int index = 0;
            // Check if the device support extensions
            if (check_device_extension_support(physical_devices_[i].physical_device_, _device_extensions) && physical_devices_[i].device_features_.samplerAnisotropy)
            {
                for (uint32_t j = 0; j < physical_devices_[i].queuefamily_properties_.size(); j++)
                {
                    if (physical_devices_[i].queuefamily_properties_[j].queueFlags & _required_queue_type)
                    {
                        indices.graphics_family_ = index;
                    }
                    if (physical_devices_[i].queuefamily_supports_present_[j])
                    {
                        indices.present_family_ = index;
                    }
                    if (indices.graphics_family_.has_value() && indices.present_family_.has_value())
                    {
                        selected_device_index_ = i;
                        break;
                    }
                    index++;
                }
            }
            
        }
        if (selected_device_index_ == -1)
        {
            throw std::runtime_error("There is no any device supports all extentions or may be supports extentions but there is no any suitable queue family !");
        }    
        return indices;
    }
    PhysicalDevice& PhysicalDevices::get_selected_device() 
    {
        if (selected_device_index_ < 0)
        {
            std::cerr << "A physical device has not been selected" << std::endl;
        }
        return physical_devices_[selected_device_index_];
    }
    bool PhysicalDevices::check_device_extension_support(VkPhysicalDevice _device, const std::vector<const char *> &_device_extensions)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());
        std::set<std::string> requiredExtensions(_device_extensions.begin(), _device_extensions.end());
        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }
} 
