#include "minerva/Device.hpp"

#include <vector>

#include "volk.h"

#include "vk/Context.hpp"
#include "vk/Instance.hpp"
#include "vk/Settings.hpp"

namespace minerva {

bool isDeviceSuitable(VkPhysicalDevice device) {
	//Check for the following things:
	// - Compute queue
	// - timeline semaphore support

	//check for timeline support
	VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	VkPhysicalDeviceVulkan12Features features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features.pNext = &features12;
	vkGetPhysicalDeviceFeatures2(device, &features);
	if (!features12.timelineSemaphore)
		return false;

	//check for compute queue
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> qProps(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, qProps.data());
	bool foundQuque = false;
	for (auto& queue : qProps) {
		if ((queue.queueFlags & Settings::QueueFlags) == Settings::QueueFlags) {
			foundQuque = true;
			break;
		}
	}
	if (!foundQuque)
		return false;

	//Everything fine
	return true;
}

std::vector<Device> enumerateDevices(const InstanceHandle& instance) {
	//enumerate devices
	uint32_t count ;
	vkEnumeratePhysicalDevices(instance->instance, &count, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(count);
	vkEnumeratePhysicalDevices(instance->instance, &count, physicalDevices.data());

	//convert to device
	std::vector<Device> devices{};
	VkPhysicalDeviceProperties props;
	for (auto pDevice : physicalDevices) {
		//suitable?
		if (!isDeviceSuitable(pDevice))
			continue;

		vkGetPhysicalDeviceProperties(pDevice, &props);

		devices.push_back({
			props.deviceName,
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
			props.vendorID,
			props.deviceID,
			static_cast<void*>(pDevice)
		});
	}

	return devices;
}

ContextHandle createContext(const InstanceHandle& instance, const Device& device) {
	return vulkan::createContext(
		instance->instance,
		static_cast<VkPhysicalDevice>(device.physicalDevice));
}

Device getDevice(const ContextHandle& context) {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(context->physicalDevice, &props);

	return {
			props.deviceName,
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
			props.vendorID,
			props.deviceID,
			static_cast<void*>(context->physicalDevice)
	};
}

}
