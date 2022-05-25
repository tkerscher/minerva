#pragma once

#include <string>
#include <vector>

#include "minerva/Instance.hpp"

namespace minerva {

struct Device {
	std::string name;
	bool isDiscrete;

	uint32_t vendorID;
	uint32_t deviceID;

	//Internal
	void* PhysicalDevice;
};

[[nodiscard]] MINERVA_API std::vector<Device> enumerateDevices(const InstanceHandle& instance);

}
