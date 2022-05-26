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

}
