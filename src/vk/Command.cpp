#include "vk/Command.hpp"

#include "volk.h"

#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"

namespace minerva::vulkan {

CommandHandle createCommand(const Context& context, bool startRecording) {
	CommandHandle result{
		new Command({nullptr, context}),
		destroyCommand
	};

	auto info = CommandBufferAllocateInfo(context.cmdPool);
	checkResult(context.table.vkAllocateCommandBuffers(
		context.device, &info, &result->buffer));

	if (startRecording) {
		auto begin = vulkan::CommandBufferBeginInfo();
		checkResult(context.table.vkBeginCommandBuffer(result->buffer, &begin));
	}

	return result;
}

void destroyCommand(Command* command) {
	if (!command)
		return;

	command->context.table.vkFreeCommandBuffers(
		command->context.device,
		command->context.cmdPool,
		1, &command->buffer);
}

}
