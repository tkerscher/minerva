#include "vk/Instance.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "volk.h"

#include "vk/result.hpp"
#include "vk/Settings.hpp"
#include "vk/Structs.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT messageSeverity,
	VkDebugReportObjectTypeEXT objType,
	uint64_t srcObject,
	size_t location,
	int32_t msgCode,
	const char* pLayerPrefix,
	const char* pMsg,
	void* pUserData) {

	//Create prefix
	std::string prefix("");
	if (messageSeverity & VK_DEBUG_REPORT_DEBUG_BIT_EXT) prefix += "[DEBUG]";
	if (messageSeverity & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) prefix += "[INFO]";
	if (messageSeverity & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) prefix += "[PERF]";
	if (messageSeverity & VK_DEBUG_REPORT_WARNING_BIT_EXT) prefix += "[WARN]";
	if (messageSeverity & VK_DEBUG_REPORT_ERROR_BIT_EXT) prefix += "[ERR]";

	//Create message
	std::stringstream message;
	message << prefix << "(" << pLayerPrefix << ") Code " << msgCode << " : " << pMsg;

	//Output
	if (messageSeverity & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		std::cerr << message.str() << std::endl;
	}
	else {
		std::cout << message.str() << std::endl;
	}

	//By returning false, the program will continue
	return VK_FALSE;
}

namespace minerva::vulkan {

InstanceHandle createInstance(const char* name, uint32_t version) {
	if (volkInitialize() != VK_SUCCESS)
		return { nullptr, destroyInstance };

	auto result = InstanceHandle(new Instance(), destroyInstance);

	auto appInfo = vulkan::ApplicationInfo(name, version);
	auto instanceInfo = vulkan::InstanceCreateInfo(appInfo);
	checkResult(vkCreateInstance(&instanceInfo, nullptr, &result->instance));

	volkLoadInstanceOnly(result->instance);

#ifdef MINERVA_DEBUG
	VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
	debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugReportCreateInfo.flags = Settings::DebugFlags;
	debugReportCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debugCallback;
	checkResult(vkCreateDebugReportCallbackEXT(
		result->instance, &debugReportCreateInfo, nullptr, &result->debugReport));
#endif

	return result;
}

void destroyInstance(Instance* instance) {
	if (!instance)
		return;

	//instance might be incomplete
	//in most cases we're save to call destroy with nullptr
#ifdef MINERVA_DEBUG
	if (instance->instance) {
		vkDestroyDebugReportCallbackEXT(instance->instance, instance->debugReport, nullptr);
	}
#endif

	vkDestroyInstance(instance->instance, nullptr);
}

}
