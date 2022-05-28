#pragma once

#define VK_NO_PROTOTYPES
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#include "volk.h"

#include "minerva/Handles.hpp"

namespace minerva::vulkan {

struct Context {
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;

	VkCommandPool cmdPool;
	VkDescriptorPool dscPool;

	VkPipelineCache pipeCache;

	VolkDeviceTable table;

	VmaAllocator allocator;
};

[[nodiscard]]
ContextHandle createContext(VkInstance instance, VkPhysicalDevice device);
void destroyContext(Context* context);

}
