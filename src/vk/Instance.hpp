#pragma once

#include <vector>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "minerva/config.hpp"
#include "minerva/Handles.hpp"

namespace minerva::vulkan {
    struct Instance {
        VkInstance instance;
#ifdef MINERVA_DEBUG
        VkDebugReportCallbackEXT debugReport;
#endif
    };

    [[nodiscard]]
    InstanceHandle createInstance(const char* name, uint32_t version);
    void destroyInstance(Instance* instance);
}
