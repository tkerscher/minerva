#pragma once

#include <vector>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "minerva/config.hpp"

namespace minerva::vulkan {
    struct Instance {
        VkInstance instance;
#ifdef MINERVA_DEBUG
        VkDebugReportCallbackEXT debugReport;
#endif
    };

    [[nodiscard]]
    Instance* createInstance(const char* name, uint32_t version);
    void destroyInstance(Instance* instance);
}
