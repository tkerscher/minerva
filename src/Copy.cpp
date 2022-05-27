#include "minerva/Copy.hpp"

#include <stdexcept>

#include "volk.h"

#include "vk/Buffer.hpp"
#include "vk/Command.hpp"
#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"

constexpr auto DIFFERENT_CONTEXT_ERROR_STR =
	"Source and destination of a copy command must originate from the same context!";
constexpr auto SIZE_MISMATCH_ERROR_STR =
	"Source and destination must have the same size!";

namespace minerva {

CommandHandle CopyCommandFactory::retrieveTensor(const TensorImp& src, const BufferImp& dst) {
	//check for src and dst to be from the same context
	if (&src.buffer->context != &dst.buffer->context)
		throw std::logic_error(DIFFERENT_CONTEXT_ERROR_STR);
	auto& context = src.buffer->context;
	//check both have the same size
	if (src.size_bytes() != dst.size_bytes())
		throw std::logic_error(SIZE_MISMATCH_ERROR_STR);
	auto size = src.size_bytes();

	//create new empty command
	auto command = vulkan::createCommand(context);
	//and start recording
	auto beginInfo = vulkan::CommandBufferBeginInfo();
	vulkan::checkResult(context.table.vkBeginCommandBuffer(command->buffer, &beginInfo));

	//ensure writing to tensor finished
	auto barrier = vulkan::BufferMemoryBarrier(src.buffer->buffer,
		VK_ACCESS_SHADER_WRITE_BIT,
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
	context.table.vkCmdCopyBuffer(command->buffer, src.buffer->buffer, dst.buffer->buffer, 1, &copyRegion);

	//barrier to ensure transfer finished
	barrier = vulkan::BufferMemoryBarrier(src.buffer->buffer,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_ACCESS_SHADER_WRITE_BIT);
	context.table.vkCmdPipelineBarrier(command->buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	//finished recording
	vulkan::checkResult(context.table.vkEndCommandBuffer(command->buffer));

	//return
	return command;
}

CommandHandle CopyCommandFactory::updateTensor(const BufferImp& src, const TensorImp& dst) {
	//check for src and dst to be from the same context
	if (&src.buffer->context != &dst.buffer->context)
		throw std::logic_error(DIFFERENT_CONTEXT_ERROR_STR);
	auto& context = src.buffer->context;
	//check both have the same size
	if (src.size_bytes() != dst.size_bytes())
		throw std::logic_error(SIZE_MISMATCH_ERROR_STR);
	auto size = src.size_bytes();

	//create new empty command
	auto command = vulkan::createCommand(context);
	//and start recording
	auto beginInfo = vulkan::CommandBufferBeginInfo();
	vulkan::checkResult(context.table.vkBeginCommandBuffer(command->buffer, &beginInfo));

	//ensure tensor is safe to update
	auto barrier = vulkan::BufferMemoryBarrier(dst.buffer->buffer,
		VK_ACCESS_SHADER_READ_BIT,
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
		src.buffer->buffer,
		dst.buffer->buffer,
		1, &copy);

	//ensure transfer completed
	barrier = vulkan::BufferMemoryBarrier(dst.buffer->buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_SHADER_READ_BIT);
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
