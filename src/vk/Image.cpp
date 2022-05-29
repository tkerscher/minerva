#include "vk/Image.hpp"

#include "volk.h"

#include "vk/Command.hpp"
#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"

namespace minerva::vulkan {

//Helper
ImageHandle createImage(const Context& context, const VkImageCreateInfo& createInfo) {
	ImageHandle result{ new Image({0,0,0,context}), destroyImage };

	//create image
	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	checkResult(vmaCreateImage(context.allocator,
		&createInfo, &allocInfo,
		&result->image, &result->allocation, nullptr));
	//create view
	auto viewInfo = ImageViewCreateInfo(result->image,
		static_cast<VkImageViewType>(createInfo.imageType),
		createInfo.format);
	checkResult(context.table.vkCreateImageView(context.device,
		&viewInfo, nullptr, &result->view));

	//This is a bit ugly. We dont know if we want to write to this image, or if it
	//will be initialized by a transfer. Either way, we would have to keep track,
	//wether the image is still undefined. Instead we will transfer the layout here.

	auto command = vulkan::createCommand(context);
	auto barrier = vulkan::ImageMemoryBarrier(
		result->image,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_GENERAL,
		0, 0);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		0, nullptr,
		1, &barrier);
	vulkan::checkResult(context.table.vkEndCommandBuffer(command->buffer));
	
	auto submit = vulkan::SubmitInfo(command->buffer);
	vulkan::checkResult(context.table.vkQueueSubmit(context.queue, 1, &submit, nullptr));

	//Right now command would be destroyed once this function returns, but it might
	//not have finished yet, resulting in undefined behaviour. We therefore release the
	//handle, resulting in some lost memory, that will get correctly returned once the
	//corresponding command buffer pool is destroyed.

	//TODO: Try to be smarter here
	command.release();

	return result;
}

ImageHandle createImage(const Context& context, VkFormat format, uint32_t width) {
	auto info = ImageCreateInfo(VK_IMAGE_TYPE_1D, { width, 1, 1 }, format);
	return createImage(context, info);
}

ImageHandle createImage(const Context& context,
	VkFormat format, uint32_t width, uint32_t height)
{
	auto info = ImageCreateInfo(VK_IMAGE_TYPE_2D, { width, height, 1 }, format);
	return createImage(context, info);
}

ImageHandle createImage(const Context& context,
	VkFormat format, uint32_t width, uint32_t height, uint32_t depth)
{
	auto info = ImageCreateInfo(VK_IMAGE_TYPE_3D, { width, height, depth }, format);
	return createImage(context, info);
}

void destroyImage(Image* image) {
	image->context.table.vkDestroyImageView(
		image->context.device, image->view, nullptr);
	vmaDestroyImage(image->context.allocator, image->image, image->allocation);
}

}
