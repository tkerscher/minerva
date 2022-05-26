#include "vk/Context.hpp"

#include <iostream>
#include <vector>

#include "vk/Structs.hpp"
#include "vk/result.hpp"

namespace minerva::vulkan {

ContextHandle createContext(VkInstance instance, VkPhysicalDevice device) {
	//query queue families
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> props(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, props.data());

	//One can get really sophisticated here by trying to select the best queue
	//Since it won't matter that much, just take the first suitable one
	uint32_t family = 0;
	for (; family < count; ++family) {
		if (props[family].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
			break;
	}

	ContextHandle context{ new Context(), destroyContext };
	context->physicalDevice = device;

	//create logical device
	auto queueInfo = vulkan::DeviceQueueCreateInfo(family);
	auto deviceInfo = vulkan::DeviceCreateInfo(queueInfo);
	checkResult(vkCreateDevice(device, &deviceInfo, nullptr, &context->device));

	//load functions
	volkLoadDeviceTable(&context->table, context->device);

	//get queue
	context->table.vkGetDeviceQueue(context->device, family, 0, &context->queue);

	//create allocator
	VmaVulkanFunctions functions{};
	functions.vkAllocateMemory                        = context->table.vkAllocateMemory;
	functions.vkBindBufferMemory                      = context->table.vkBindBufferMemory;
	functions.vkBindBufferMemory2KHR                  = context->table.vkBindBufferMemory2;
	functions.vkBindImageMemory                       = context->table.vkBindImageMemory;
	functions.vkBindImageMemory2KHR                   = context->table.vkBindImageMemory2;
	functions.vkCmdCopyBuffer                         = context->table.vkCmdCopyBuffer;
	functions.vkCreateBuffer                          = context->table.vkCreateBuffer;
	functions.vkCreateImage                           = context->table.vkCreateImage;
	functions.vkDestroyBuffer                         = context->table.vkDestroyBuffer;
	functions.vkDestroyImage                          = context->table.vkDestroyImage;
	functions.vkFlushMappedMemoryRanges               = context->table.vkFlushMappedMemoryRanges;
	functions.vkFreeMemory                            = context->table.vkFreeMemory;
	functions.vkGetBufferMemoryRequirements           = context->table.vkGetBufferMemoryRequirements;
	functions.vkGetBufferMemoryRequirements2KHR       = context->table.vkGetBufferMemoryRequirements2;
	functions.vkGetImageMemoryRequirements            = context->table.vkGetImageMemoryRequirements;
	functions.vkGetImageMemoryRequirements2KHR        = context->table.vkGetImageMemoryRequirements2;
	functions.vkGetPhysicalDeviceMemoryProperties     = vkGetPhysicalDeviceMemoryProperties;
	functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;
	functions.vkGetPhysicalDeviceProperties           = vkGetPhysicalDeviceProperties;
	functions.vkInvalidateMappedMemoryRanges          = context->table.vkInvalidateMappedMemoryRanges;
	functions.vkMapMemory                             = context->table.vkMapMemory;
	functions.vkUnmapMemory                           = context->table.vkUnmapMemory;
	//Vulkan 1.3 or VK_KHR_maintenance4
	//functions.vkGetDeviceBufferMemoryRequirements
	//functions.vkGetDeviceImageMemoryRequirements

	VmaAllocatorCreateInfo info{};
	info.vulkanApiVersion = VK_API_VERSION_1_1;
	info.physicalDevice = device;
	info.device = context->device;
	info.instance = instance;
	info.pVulkanFunctions = &functions;
	checkResult(vmaCreateAllocator(&info, &context->allocator));

	//done
	return context;
}

void destroyContext(Context* context) {
	if (!context)
		return;

	vmaDestroyAllocator(context->allocator);
	context->table.vkDestroyDevice(context->device, nullptr);
}

}