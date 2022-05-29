#pragma once

#include "minerva/Device.hpp"
#include "minerva/ImageFormat.hpp"
#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API Image {
	public:
		[[nodiscard]] uint32_t getWidth() const;
		[[nodiscard]] uint32_t getHeight() const;
		[[nodiscard]] uint32_t getDepth() const;

		[[nodiscard]] ImageFormat getFormat() const;
		[[nodiscard]] uint64_t size_bytes() const;

		Image(const ContextHandle& context,
			ImageFormat format,
			uint32_t width);
		Image(const ContextHandle& context,
			ImageFormat format,
			uint32_t width, uint32_t height);
		Image(const ContextHandle& context,
			ImageFormat format,
			uint32_t width, uint32_t height, uint32_t depth);
		~Image();

	public: //Internal
		const vulkan::Image& getImage() const;

	private:
		ImageHandle image;
		ImageFormat _format;
		uint32_t _width, _height, _depth;
	};

	[[nodiscard]] uint64_t MINERVA_API getElementSize(ImageFormat format);
}
