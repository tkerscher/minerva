#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "vk/Settings.hpp"

namespace minerva::vulkan {

[[nodiscard]]
constexpr VkApplicationInfo ApplicationInfo(
	const char* name,
	uint32_t version)
{
	VkApplicationInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = name;
	info.applicationVersion = version;
	info.pEngineName = Settings::EngineName;
	info.engineVersion = Settings::EngineVersion;
	info.apiVersion = Settings::ApiVersion;

	return info;
}

[[nodiscard]]
constexpr VkBufferCreateInfo BufferCreateInfo(
	uint64_t size,
	VkBufferUsageFlags usage)
{
	VkBufferCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.size = size;
	info.usage = usage;

	return info;
}


[[nodiscard]]
constexpr VkDeviceCreateInfo DeviceCreateInfo(
	const VkDeviceQueueCreateInfo& queue)
{
	VkDeviceCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pQueueCreateInfos = &queue;
	info.queueCreateInfoCount = 1;
	info.pEnabledFeatures = &Settings::DeviceFeatures;
	info.ppEnabledExtensionNames = Settings::DeviceExtensions.data();
	info.enabledExtensionCount = static_cast<uint32_t>(Settings::DeviceExtensions.size());
	info.ppEnabledLayerNames = nullptr; //they're deprecated
	info.enabledLayerCount = 0;

	return info;
}

[[nodiscard]]
constexpr VkDeviceQueueCreateInfo DeviceQueueCreateInfo(
	uint32_t index)
{
	VkDeviceQueueCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	info.queueCount = 1;
	info.queueFamilyIndex = index;
	info.pQueuePriorities = &Settings::QueuePriority;

	return info;
}

[[nodiscard]]
constexpr VkInstanceCreateInfo InstanceCreateInfo(
	const VkApplicationInfo& appInfo)
{
	VkInstanceCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.ppEnabledExtensionNames = Settings::InstanceExtensions.data();
	info.enabledExtensionCount = Settings::InstanceExtensions.size();
	info.ppEnabledLayerNames = Settings::Layers.data();
	info.enabledLayerCount = Settings::Layers.size();
	info.pApplicationInfo = &appInfo;

	return info;
}

[[nodiscard]]
constexpr VkSemaphoreCreateInfo SemaphoreCreateInfo(
	const VkSemaphoreTypeCreateInfo& typeInfo)
{
	VkSemaphoreCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = &typeInfo;

	return info;
}

[[nodiscard]]
constexpr VkSemaphoreSignalInfo SemaphoreSignalInfo(
	VkSemaphore semaphore,
	uint64_t value)
{
	VkSemaphoreSignalInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
	info.semaphore = semaphore;
	info.value = value;

	return info;
}

[[nodiscard]]
constexpr VkSemaphoreTypeCreateInfo SemaphoreTypeCreateInfo(
	uint64_t initialValue)
{
	VkSemaphoreTypeCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	info.initialValue = initialValue;

	return info;
}

[[nodiscard]]
constexpr VkSemaphoreWaitInfo SemaphoreWaitInfo(
	const VkSemaphore& semaphore,
	const uint64_t& value)
{
	VkSemaphoreWaitInfo info = {};
	
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.semaphoreCount = 1;
	info.pSemaphores = &semaphore;
	info.pValues = &value;

	return info;
}

}
