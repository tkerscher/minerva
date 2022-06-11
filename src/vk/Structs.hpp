#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "minerva/span.hpp"
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
constexpr VkBufferImageCopy BufferImageCopy(
	VkOffset3D offset,
	VkExtent3D extent)
{
	VkBufferImageCopy copy = {};

	copy.bufferRowLength = 0;   //tightly packed
	copy.bufferImageHeight = 0; //tightly packed
	copy.imageSubresource = {
		VK_IMAGE_ASPECT_COLOR_BIT,
		0, 0, 1 };
	copy.imageOffset = offset;
	copy.imageExtent = extent;

	return copy;
}

[[nodiscard]]
constexpr VkBufferMemoryBarrier BufferMemoryBarrier(
	VkBuffer buffer,
	VkAccessFlags src,
	VkAccessFlags dst)
{
	VkBufferMemoryBarrier barrier = {};

	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.buffer = buffer;
	barrier.srcAccessMask = src;
	barrier.dstAccessMask = dst;
	barrier.size = VK_WHOLE_SIZE;

	return barrier;
}

[[nodiscard]]
constexpr VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
	VkCommandPool pool)
{
	VkCommandBufferAllocateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandPool = pool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	return info;
}

[[nodiscard]]
constexpr VkCommandBufferBeginInfo CommandBufferBeginInfo()
{
	VkCommandBufferBeginInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	return info;
}

[[nodiscard]]
constexpr VkCommandPoolCreateInfo CommandPoolCreateInfo(
	uint32_t family)
{
	VkCommandPoolCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.flags = Settings::CommandPoolFlags;
	info.queueFamilyIndex = family;

	return info;
}

[[nodiscard]]
constexpr VkComputePipelineCreateInfo ComputePipelineCreateInfo(
	VkPipelineLayout layout,
	const char* entryPoint,
	VkShaderModule shaderModule,
	const VkSpecializationInfo* specInfo)
{
	VkComputePipelineCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.layout = layout;

	//special: stage is not a pointer, but nested
	info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.stage.module = shaderModule;
	info.stage.pName = entryPoint;
	info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	info.stage.pSpecializationInfo = specInfo;

	return info;
}

[[nodiscard]]
constexpr VkDescriptorPoolCreateInfo DescriptorPoolInfo()
{
	VkDescriptorPoolCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.flags = Settings::DescriptorPoolCreateFlags;
	info.maxSets = Settings::MaxDescriptorPoolSets;
	info.poolSizeCount = static_cast<uint32_t>(Settings::DescriptorPoolSizes.size());
	info.pPoolSizes = Settings::DescriptorPoolSizes.data();

	return info;
}

[[nodiscard]]
constexpr VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(
	VkDescriptorPool pool,
	const VkDescriptorSetLayout& layout)
{
	VkDescriptorSetAllocateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = pool;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &layout;

	return info;
}

[[nodiscard]]
constexpr VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(
	span<VkDescriptorSetLayoutBinding> bindings)
{
	VkDescriptorSetLayoutCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.flags = Settings::DescriptorSetLayoutCreateFlags;
	info.bindingCount = static_cast<uint32_t>(bindings.size());
	info.pBindings = bindings.data();

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
constexpr VkImageCreateInfo ImageCreateInfo(
	VkImageType type,
	VkExtent3D extent,
	VkFormat format)
{
	VkImageCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = type;
	info.extent = extent;
	info.format = format;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = Settings::ImageUsage;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	return info;
}

[[nodiscard]]
constexpr VkImageMemoryBarrier ImageMemoryBarrier(
	VkImage image,
	VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags srcAccess, VkAccessFlags dstAccess)
{
	VkImageMemoryBarrier barrier = {};

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange = {
		VK_IMAGE_ASPECT_COLOR_BIT,
		0, 1,
		0, 1
	};

	return barrier;
}

[[nodiscard]]
constexpr VkImageViewCreateInfo ImageViewCreateInfo(
	VkImage image,
	VkImageViewType type,
	VkFormat format)
{
	VkImageViewCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.image = image;
	info.viewType = type;
	info.format = format;
	info.subresourceRange = {
		VK_IMAGE_ASPECT_COLOR_BIT, //TODO: I'm unsure if this should be NONE
		0, 1, 0, 1
	};
	//info.components //leaving this zero means identity

	return info;
}

[[nodiscard]]
constexpr VkInstanceCreateInfo InstanceCreateInfo(
	const VkApplicationInfo& appInfo)
{
	VkInstanceCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.ppEnabledExtensionNames = Settings::InstanceExtensions.data();
	info.enabledExtensionCount = static_cast<uint32_t>(Settings::InstanceExtensions.size());
	info.ppEnabledLayerNames = Settings::Layers.data();
	info.enabledLayerCount = static_cast<uint32_t>(Settings::Layers.size());
	info.pApplicationInfo = &appInfo;

	return info;
}

[[nodiscard]]
constexpr VkPipelineCacheCreateInfo PipelineCacheCreateInfo()
{
	VkPipelineCacheCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	
	return info;
}

[[nodiscard]]
constexpr VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(
	span<VkDescriptorSetLayout> sets)
{
	VkPipelineLayoutCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.setLayoutCount = static_cast<uint32_t>(sets.size());
	info.pSetLayouts = sets.data();
	
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

[[nodiscard]]
constexpr VkShaderModuleCreateInfo ShaderModuleCreateInfo(
	span<const uint32_t> code)
{
	VkShaderModuleCreateInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = code.size_bytes();
	info.pCode = code.data();

	return info;
}

[[nodiscard]]
constexpr VkSpecializationInfo SpecializationInfo(
	span<VkSpecializationMapEntry> entries,
	span<const std::byte> data)
{
	VkSpecializationInfo info = {};

	info.mapEntryCount = static_cast<uint32_t>(entries.size());
	info.pMapEntries = entries.data();
	info.dataSize = static_cast<uint32_t>(data.size());
	info.pData = data.data();

	return info;
}

[[nodiscard]]
constexpr VkSubmitInfo SubmitInfo(
	const VkSemaphore& semaphore,
	const VkPipelineStageFlags* waitDstStageMask,
	const VkTimelineSemaphoreSubmitInfo* timeline,
	span<const VkCommandBuffer> cmdBuffers)
{
	VkSubmitInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = timeline;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &semaphore;
	info.pWaitDstStageMask = waitDstStageMask;
	info.signalSemaphoreCount = 1;
	info.pSignalSemaphores = &semaphore;
	info.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());
	info.pCommandBuffers = cmdBuffers.data();

	return info;
}

[[nodiscard]]
constexpr VkSubmitInfo SubmitInfo(
	const VkCommandBuffer& cmdBuf)
{
	VkSubmitInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.commandBufferCount = 1;
	info.pCommandBuffers = &cmdBuf;

	return info;
}

[[nodiscard]]
constexpr VkTimelineSemaphoreSubmitInfo TimelineSemaphoreSubmitInfo(
	uint64_t* waitValue,
	uint64_t* signalValue)
{
	VkTimelineSemaphoreSubmitInfo info = {};

	info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
	info.waitSemaphoreValueCount = 1;
	info.pWaitSemaphoreValues = waitValue;
	info.signalSemaphoreValueCount = 1;
	info.pSignalSemaphoreValues = signalValue;

	return info;
}

[[nodiscard]]
constexpr VkWriteDescriptorSet WriteDescriptorSet(
	VkDescriptorSet set,
	uint32_t dstBinding,
	VkDescriptorType type,
	const VkDescriptorBufferInfo& bufferInfo)
{
	VkWriteDescriptorSet info = {};

	info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	info.dstSet = set;
	info.dstBinding = dstBinding;
	info.descriptorType = type;
	info.descriptorCount = 1;
	info.pBufferInfo = &bufferInfo;

	return info;
}

[[nodiscard]]
constexpr VkWriteDescriptorSet WriteDescriptorSet(
	VkDescriptorSet set,
	uint32_t dstBinding,
	VkDescriptorType type,
	const VkDescriptorImageInfo& imageInfo)
{
	VkWriteDescriptorSet info = {};

	info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	info.dstSet = set;
	info.dstBinding = dstBinding;
	info.descriptorType = type;
	info.descriptorCount = 1;
	info.pImageInfo = &imageInfo;

	return info;
}

}
