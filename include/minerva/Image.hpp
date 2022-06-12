#pragma once

#include "minerva/Device.hpp"
#include "minerva/ImageFormat.hpp"
#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	/**
	 * @brief Image managing memory on GPU as an image.
	 */
	class MINERVA_API Image {
	public:
		/**
		 * @brief Returns the width of the image
		 * 
		 * @return uint32_t width of the image
		 */
		[[nodiscard]] uint32_t getWidth() const;
		/**
		 * @brief Returns the height of the image. Will be 1 for 1D images.
		 * 
		 * @return uint32_t height of the image.
		 */
		[[nodiscard]] uint32_t getHeight() const;
		/**
		 * @brief Returns the depth of the image. Will be 1 for 1D or 2D images.
		 * 
		 * @return uint32_t depth of the image.
		 */
		[[nodiscard]] uint32_t getDepth() const;

		/**
		 * @brief Returns the format of the image.
		 * 
		 * @return ImageFormat Format of the image.
		 */
		[[nodiscard]] ImageFormat getFormat() const;
		/**
		 * @brief Return the size of the image in bytes when tightly packed.
		 * 
		 * @return uint64_t size in bytes
		 */
		[[nodiscard]] uint64_t size_bytes() const;

		/**
		 * @brief Construct a new 1D Image
		 * 
		 * @param context context on which to create this
		 * @param format format of the image
		 * @param width width of the image
		 */
		Image(const ContextHandle& context,
			ImageFormat format,
			uint32_t width);
		/**
		 * @brief Construct a new 2D Image
		 * 
		 * @param context context on which to create this
		 * @param format format of the image
		 * @param width width of the image
		 * @param height height of the image
		 */
		Image(const ContextHandle& context,
			ImageFormat format,
			uint32_t width, uint32_t height);
		/**
		 * @brief Construct a new 3D Image
		 * 
		 * @param context context on which to create this
		 * @param format format of the image
		 * @param width width of the image
		 * @param height height of the image
		 * @param depth depth of the image
		 */
		Image(const ContextHandle& context,
			ImageFormat format,
			uint32_t width, uint32_t height, uint32_t depth);
		~Image();

	public: //Internal
		/// @private
		const vulkan::Image& getImage() const;

	private:
		ImageHandle image;
		ImageFormat _format;
		uint32_t _width, _height, _depth;
	};

	/**
	 * @brief Returns the size of a single pixel in bytes for a given format.
	 * 
	 * @param format The format to query
	 * @return uint64_t pixel size in bytes
	 */
	[[nodiscard]] uint64_t MINERVA_API getElementSize(ImageFormat format);
}

/**
 * @class minerva::Image
 * @ingroup memory
 * 
 * Image manages a vulkan image living on the GPU. In the shader these can
 * defined like in the following depending on the number of dimensions:
 * 
 * @code {.unparsed}
 * layout(binding = 0, rgba8) uniform image2D iamge;
 * @endcode
 * 
 * To fill a image or retrieve it you need to issue a copy command on a sequence
 * to copy to or from a Buffer, like in the following:
 * 
 * @code {.cpp}
 * Image image(context, ImageFormat::R8G8B8A8_UNORM, width, height);
 * ImageBuffer buffer(context, width, height);
 * 
 * auto copyTo = updateImage(buffer, image);
 * auto copyFrom = retrieveImage(image, buffer);
 * 
 * Timeline timeline(context);
 * beginSequence(timeline)
 * 		.And(copyTo)
 * 		.Then(copyFrom)
 * 		.Submit();
 * @endcode
 * 
 * @see Buffer
 * @see updateImage
 * @see retrieveImage
 */
