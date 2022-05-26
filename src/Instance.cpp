#include "minerva/Instance.hpp"

#include "minerva/Device.hpp"
#include "vk/Context.hpp"
#include "vk/Instance.hpp"

namespace minerva {

InstanceHandle createInstance() {
	return createInstance("Minerva App", { 1,0,0 });
}

InstanceHandle createInstance(const char* name, AppVersion version) {
	return vulkan::createInstance(name, version.packed);
}

ContextHandle createContext(const InstanceHandle& instance) {
	return createContext(instance, "Minerva App", { 1,0,0 });
}

ContextHandle createContext(
	const InstanceHandle& instance,
	const char* name,
	AppVersion version)
{
	auto devices = enumerateDevices(instance);

	//any devices found?
	if (devices.empty())
		return { nullptr, vulkan::destroyContext };

	//select first discrete GPU
	for (auto& device : devices) {
		if (device.isDiscrete) {
			return vulkan::createContext(
				instance->instance,
				static_cast<VkPhysicalDevice>(device.physicalDevice));
		}
	}

	//no discrete gpu found -> return first one
	return vulkan::createContext(
		instance->instance,
		static_cast<VkPhysicalDevice>(devices[0].physicalDevice));
}

ContextHandle createContext(
	const InstanceHandle& instance,
	uint32_t vendorId,
	uint32_t deviceId)
{
	return createContext(instance, "Minerva App", { 1,0,0 }, vendorId, deviceId);
}

ContextHandle createContext(
	const InstanceHandle& instance,
	const char* name,
	AppVersion version,
	uint32_t vendorId,
	uint32_t deviceId)
{
	auto devices = enumerateDevices(instance);

	//search devices
	for (auto& device : devices) {
		if (device.vendorID == vendorId && device.deviceID == deviceId)
			return createContext(instance, device);
	}

	//Nothing found
	return { nullptr, vulkan::destroyContext };
}

}
