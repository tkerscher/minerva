#include "minerva/ImageBuffer.hpp"

#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace minerva {

uint32_t ImageBuffer::getWidth() const {
	return width;
}

uint32_t ImageBuffer::getHeight() const {
	return height;
}

//TODO: It's a bit ugly that the context needs to be passed again,
//but creating a non owning ContextHandle feels wrong
Image ImageBuffer::createImage(const ContextHandle& context) const {
	return Image(context, Format, getWidth(), getHeight());
}

ImageBuffer ImageBuffer::load(const ContextHandle& context, const char* filename) {
	//load image
	//unfortunately, stbi wants to allocate its own memory, so we have an extra copy...
	int x,y,n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, STBI_rgb_alpha);
	ImageBuffer result(context, x, y);
	auto mem = result.getMemory();
	
	//copy image and free data
	memcpy(mem.data(), data, result.size_bytes());
	stbi_image_free(data);

	//done
	return result;
}

ImageBuffer ImageBuffer::load(const ContextHandle& context, span<const std::byte> memory) {
	int x, y, n;
	auto data = stbi_load_from_memory(
		reinterpret_cast<const unsigned char*>(memory.data()),
		static_cast<int>(memory.size_bytes()),
		&x, &y, &n, STBI_rgb_alpha);
	ImageBuffer result(context, x, y);
	auto mem = result.getMemory();

	memcpy(mem.data(), data, mem.size_bytes());
	stbi_image_free(data);

	return result;
}

void ImageBuffer::save(const char* filename) const {
	stbi_write_png(filename,
		getWidth(), getHeight(), STBI_rgb_alpha,
		getMemory().data(),
		getWidth() * 4);
}

ImageBuffer::ImageBuffer(const ContextHandle& context, uint32_t width, uint32_t height)
	: Buffer(context, width * height)
	, width(width), height(height)
{}

}
