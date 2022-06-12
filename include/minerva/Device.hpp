#pragma once

#include <string>
#include <vector>

#include "minerva/Instance.hpp"

namespace minerva {

/**
 * @brief Vulkan capable device
 * @ingroup core
 */
struct Device {
	std::string name; //!< Name of the device
	bool isDiscrete;  //!< True, if the device is a discrete GPU

	uint32_t vendorID; //!< ID of vendor
	uint32_t deviceID; //!< ID of device

	//Internal
	/// @private
	void* physicalDevice;
};

/**
 * @brief Enumerates a vector of Device capable of Vulkan
 * @ingroup core
 * 
 * @param instance Instance used to query devices
 * @return std::vector<Device> vector of devices
 */
[[nodiscard]] MINERVA_API std::vector<Device> enumerateDevices(const InstanceHandle& instance);
/**
 * @brief Create a Context object
 * @ingroup core
 * 
 * @param instance Instance on which to create the context
 * @param device Device on which the context will be created
 * @return ContextHandle Handle to the created Context
 */
[[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance, const Device& device);
/**
 * @brief Get the Device on which the context was created
 * @ingroup core
 * 
 * @param context context to query
 * @return Device the context lives on
 */
[[nodiscard]] MINERVA_API Device getDevice(const ContextHandle& context);

}
