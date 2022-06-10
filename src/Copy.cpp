#include "minerva/Copy.hpp"

#include <stdexcept>

#include "volk.h"

#include "vk/Buffer.hpp"
#include "vk/Command.hpp"
#include "vk/Context.hpp"
#include "vk/Image.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"
#include "array.hpp"

constexpr auto DIFFERENT_CONTEXT_ERROR_STR =
	"Source and destination of a copy command must originate from the same context!";
constexpr auto SIZE_MISMATCH_ERROR_STR =
	"Source and destination must have the same size!";

namespace minerva {

CommandHandle retrieveImage(const Image& src, const BufferImp& dst) {
	//check for src and dst to be from the same context
	if (&src.getImage().context != &dst.getBuffer().context)
		throw std::logic_error(DIFFERENT_CONTEXT_ERROR_STR);
	auto& context = src.getImage().context;
	//check that buffer has the right size
	if (src.size_bytes() != dst.size_bytes())
		throw std::logic_error(SIZE_MISMATCH_ERROR_STR);

	//Create new empty command
	auto command = vulkan::createCommand(context);

	//ensure writing to image finished and prepare image as transfer src
	auto barrier = vulkan::ImageMemoryBarrier(
		src.getImage().image,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VK_ACCESS_TRANSFER_READ_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	//actuall copy the image to buffer
	auto copy = vulkan::BufferImageCopy(
		{ 0, 0, 0 }, { src.getWidth(), src.getHeight(), src.getDepth() });
	context.table.vkCmdCopyImageToBuffer(command->buffer,
		src.getImage().image,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		dst.getBuffer().buffer,
		1, &copy);

	//ensure transfer finished and make buffer coherent
	barrier = vulkan::ImageMemoryBarrier(
		src.getImage().image,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		0, 0);
	auto bufferBarrier = vulkan::BufferMemoryBarrier(
		dst.getBuffer().buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_HOST_READ_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		1, &bufferBarrier,
		1, &barrier);

	//finish recording and return
	vulkan::checkResult(context.table.vkEndCommandBuffer(command->buffer));
	return command;
}

CommandHandle retrieveTensor(const TensorImp& src, const BufferImp& dst) {
	//check for src and dst to be from the same context
	if (&src.getBuffer().context != &dst.getBuffer().context)
		throw std::logic_error(DIFFERENT_CONTEXT_ERROR_STR);
	auto& context = src.getBuffer().context;
	//check both have the same size
	if (src.size_bytes() != dst.size_bytes())
		throw std::logic_error(SIZE_MISMATCH_ERROR_STR);
	auto size = src.size_bytes();

	//create new empty command
	auto command = vulkan::createCommand(context);

	//ensure writing to tensor finished
	auto barrier = vulkan::BufferMemoryBarrier(
		src.getBuffer().buffer,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VK_ACCESS_TRANSFER_READ_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	//actually copy the buffer
	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	context.table.vkCmdCopyBuffer(command->buffer,
		src.getBuffer().buffer,
		dst.getBuffer().buffer,
		1, &copyRegion);

	//barrier to ensure transfer finished
	barrier = vulkan::BufferMemoryBarrier(
		dst.getBuffer().buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_HOST_READ_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	//finished recording
	vulkan::checkResult(context.table.vkEndCommandBuffer(command->buffer));

	//return
	return command;
}

CommandHandle updateImage(const BufferImp& src, const Image& dst) {
	//check for src and dst to be from the same context
	if (&src.getBuffer().context != &dst.getImage().context)
		throw std::logic_error(DIFFERENT_CONTEXT_ERROR_STR);
	auto& context = src.getBuffer().context;
	//check that buffer has the right size
	if (src.size_bytes() != dst.size_bytes())
		throw std::logic_error(SIZE_MISMATCH_ERROR_STR);

	//Create new empty command
	auto command = vulkan::createCommand(context);

	//make ensure image is safe to write and prepare it for the transfer
	auto barrier = vulkan::ImageMemoryBarrier(
		dst.getImage().image,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	//actual copy
	auto copy = vulkan::BufferImageCopy(
		{ 0, 0, 0 }, { dst.getWidth(), dst.getHeight(), dst.getDepth() });
	context.table.vkCmdCopyBufferToImage(command->buffer,
		src.getBuffer().buffer,
		dst.getImage().image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &copy);

	//make sure the image is ready and transfer to shader comp layout
	barrier = vulkan::ImageMemoryBarrier(
		dst.getImage().image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	//finish recording and return
	vulkan::checkResult(context.table.vkEndCommandBuffer(command->buffer));
	return command;
}

CommandHandle updateTensor(const BufferImp& src, const TensorImp& dst) {
	//check for src and dst to be from the same context
	if (&src.getBuffer().context != &dst.getBuffer().context)
		throw std::logic_error(DIFFERENT_CONTEXT_ERROR_STR);
	auto& context = src.getBuffer().context;
	//check both have the same size
	if (src.size_bytes() != dst.size_bytes())
		throw std::logic_error(SIZE_MISMATCH_ERROR_STR);
	auto size = src.size_bytes();

	//create new empty command
	auto command = vulkan::createCommand(context);

	//ensure tensor is safe to update
	auto barrier = vulkan::BufferMemoryBarrier(
		dst.getBuffer().buffer,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	//copy
	VkBufferCopy copy{};
	copy.size = size;
	context.table.vkCmdCopyBuffer(command->buffer,
		src.getBuffer().buffer,
		dst.getBuffer().buffer,
		1, &copy);

	//ensure transfer completed
	barrier = vulkan::BufferMemoryBarrier(
		dst.getBuffer().buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	//finish recording
	context.table.vkEndCommandBuffer(command->buffer);

	//done
	return command;
}

}
