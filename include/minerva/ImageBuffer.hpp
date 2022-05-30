#pragma once

#include "minerva/Buffer.hpp"
#include "minerva/ImageFormat.hpp"

namespace minerva {
	class MINERVA_API ImageBuffer
		: public Buffer<Vec4<uint8_t>>
	{
	public:
		using ElementType = Vec4<uint8_t>;
		static constexpr auto Format = ImageFormat::R8G8B8A8_UNORM;

	public:
		[[nodiscard]] uint32_t getWidth() const;
		[[nodiscard]] uint32_t getHeight() const;

		[[nodiscard]] static ImageBuffer load(const ContextHandle& context, const char* filename);
		[[nodiscard]] static ImageBuffer load(const ContextHandle& context, span<const std::byte> memory);

		//always saves as png
		void save(const char* filename) const;
		//void save(span<std::byte> memory) const;

		ImageBuffer(const ContextHandle& context, uint32_t width, uint32_t height);

	private:
		uint32_t width, height;
	};
}
