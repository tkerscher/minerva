#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "array.hpp"
#include "minerva/config.hpp"

namespace minerva::Settings {

    //Engine Info
    constexpr auto EngineName = "Minerva";
    constexpr uint32_t EngineVersion = VK_MAKE_VERSION(0, 1, 0);
    constexpr uint32_t ApiVersion = VK_API_VERSION_1_1;

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

    //Queue priority
    constexpr float QueuePriority = 1.0f;

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
