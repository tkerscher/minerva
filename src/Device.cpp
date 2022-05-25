#include "minerva/Device.hpp"

#include <iostream>

#include "volk.h"

#include "vk/Instance.hpp"

namespace minerva {

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

}
