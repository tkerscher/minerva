#pragma once

#define VK_NO_PROTOTYPES
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#include "minerva/Handles.hpp"

namespace minerva::vulkan {
	struct Image {
		VkImage image;
		VkImageView view;
		VmaAllocation allocation;

		const Context& context;
	};

	[[nodiscard]] ImageHandle createImage(const Context& context,
		VkFormat format, uint32_t width);
	[[nodiscard]] ImageHandle createImage(const Context& context,
		VkFormat format, uint32_t width, uint32_t height);
	[[nodiscard]] ImageHandle createImage(const Context& context,
		VkFormat format, uint32_t width, uint32_t height, uint32_t depth);
	void destroyImage(Image* image);
}
