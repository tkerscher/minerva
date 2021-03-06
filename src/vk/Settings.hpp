#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "array.hpp"
#include "minerva/config.hpp"

namespace minerva::Settings {

    //Engine Info
    constexpr auto EngineName = "Minerva";
    constexpr uint32_t EngineVersion = VK_MAKE_VERSION(0, 1, 0);
    constexpr uint32_t ApiVersion = VK_API_VERSION_1_2;

    //Instance settings
#ifdef MINERVA_DEBUG
    constexpr auto Layers = to_array({
        "VK_LAYER_KHRONOS_validation"
    });
    constexpr auto InstanceExtensions = to_array({
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    });
#else
    constexpr std::array<const char*, 0> Layers{};
    constexpr std::array<const char*, 0> InstanceExtensions{};
#endif

    //Device settings
    constexpr std::array<const char*, 0> DeviceExtensions{};
    constexpr VkPhysicalDeviceFeatures DeviceFeatures{};
    //C++20 introduced aggregated initializer, but we're at C++17 -> use this ugly thing
    constexpr VkPhysicalDeviceVulkan12Features _12features() {
        VkPhysicalDeviceVulkan12Features features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features.timelineSemaphore = VK_TRUE;
        return features;
    }
    constexpr VkPhysicalDeviceVulkan12Features DeviceVulkan12Features = _12features();

    //Queue settings
    constexpr VkQueueFlags QueueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    constexpr float QueuePriority = 1.0f;
    constexpr VkCommandPoolCreateFlagBits CommandPoolFlags = {};

    //Descriptor Pool
    constexpr VkDescriptorSetLayoutCreateFlags DescriptorSetLayoutCreateFlags =
        VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    constexpr VkDescriptorPoolCreateFlags DescriptorPoolCreateFlags =
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT |
        VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    constexpr uint32_t MaxDescriptorPoolSets = 1000;
    constexpr auto DescriptorPoolSizes = to_array<VkDescriptorPoolSize>({
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 }
    });

    //Image settings
    constexpr VkImageUsageFlags ImageUsage =
        VK_IMAGE_USAGE_STORAGE_BIT |
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    //Debug
    constexpr VkDebugReportFlagsEXT DebugFlags =
#if defined(MINERVA_VERBOSE_VULKAN)
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT |
#endif
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT;
}
