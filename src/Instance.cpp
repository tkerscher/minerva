#include "minerva/Instance.hpp"

#include "vk/Instance.hpp"

namespace minerva {

InstanceHandle createInstance() {
	return createInstance("Minerva App", { 1,0,0 });
}

InstanceHandle createInstance(const char* name, AppVersion version) {
	return { vulkan::createInstance(name, version.packed), &vulkan::destroyInstance };
}

}
