#pragma once

#include "minerva/Buffer.hpp"
#include "minerva/Image.hpp"

namespace minerva {
	/**
	 * @brief Buffer specialized for interaction with Image.
	 */
	class MINERVA_API ImageBuffer
		: public Buffer<Vec4<uint8_t>>
	{
	public:
		/**
		 * @brief Type of the pixel 
		 */
		using ElementType = Vec4<uint8_t>;
		/**
		 * @brief Format used by ImageBuffer 
		 */
		static constexpr auto Format = ImageFormat::R8G8B8A8_UNORM;

	public:
		/**
		 * @brief Returns the width of the image
		 * 
		 * @return uint32_t with of the image
		 */
		[[nodiscard]] uint32_t getWidth() const;
		/**
		 * @brief Returns the height of the image
		 * 
		 * @return uint32_t height of the image.
		 */
		[[nodiscard]] uint32_t getHeight() const;

		/**
		 * @brief Loads a image from the given file and returns it as a new ImageBuffer.
		 * 
		 * @param context context on which to create this
		 * @param filename path to image to load
		 * @return ImageBuffer New ImageBuffer holding the loaded image
		 */
		[[nodiscard]] static ImageBuffer load(const ContextHandle& context, const char* filename);
		/**
		 * @brief Loads a image from the given memory and returns it as new ImageBuffer.
		 * 
		 * @param context context on which to create this
		 * @param memory memory holding the image
		 * @return ImageBuffer New ImageBuffer holding the loaded image
		 */
		[[nodiscard]] static ImageBuffer load(const ContextHandle& context, span<const std::byte> memory);

		/**
		 * @brief Create a Image object with the same dimension as this
		 * 
		 * @param context context on which to create this
		 * @return Image 
		 */
		[[nodiscard]] Image createImage(const ContextHandle& context) const;

		/**
		 * @brief Saves the current image as png at the given path
		 * 
		 * @param filename path to where to save the image
		 */
		void save(const char* filename) const;
		//void save(span<std::byte> memory) const;

		/**
		 * @brief Construct a new ImageBuffer
		 * 
		 * @param context context on which to create this
		 * @param width width of the image
		 * @param height height of the image
		 */
		ImageBuffer(const ContextHandle& context, uint32_t width, uint32_t height);

	private:
		uint32_t width, height;
	};
}

/**
 * @class minerva::ImageBuffer
 * @ingroup memory
 * 
 * ImageBuffer is a specialization of Buffer providing additional functions for
 * interacting with images on the filesystem. Internally it utilizes stb_image
 * and thus provides for example the following formats for loading:
 *  - PNG
 *  - JPG
 *  - BMP
 *  - GIF
 *  - PPM
 * 
 * While it will only save images as png.
 * 
 * Images will always be loaded as 4 channel RGBA 8bit per channel image.
 * 
 * @see Buffer
 * @see Image
 */
