#pragma once

#include "minerva/Buffer.hpp"
#include "minerva/Image.hpp"
#include "minerva/Handles.hpp"
#include "minerva/Tensor.hpp"

namespace minerva {

	//retrieve

	/**
	 * @brief Retrieves the given Image back from the GPU
	 * @ingroup memory
	 * 
	 * Creates a command for copying the given Image to the given Buffer, which
	 * can be issued in a sequence. They must be created on the same context and
	 * match in size.
	 * 
	 * @param src Image to copy from
	 * @param dst Buffer to copy to
	 * @return CommandHandle Command for retrieving the image 
	 */
	[[nodiscard]] MINERVA_API CommandHandle retrieveImage(const Image& src, const BufferImp& dst);
	/**
	 * @brief Retrieves the given Tensor back from the GPU
	 * @ingroup memory
	 * 
	 * Creates a command for copying the given Tensor to the given Buffer, which
	 * can be issued in a sequence. They must be created on the same context and
	 * match in size.
	 * 
	 * @param src Tensor to copy from
	 * @param dst Buffer to copy to
	 * @return CommandHandle Command for retrieving the Tensor 
	 */
	[[nodiscard]] MINERVA_API CommandHandle retrieveTensor(const TensorImp& src, const BufferImp& dst);

	//update

	/**
	 * @brief Updates the given Image using the given buffer.
	 * @ingroup memory
	 * 
	 * Creates a command for updating the given Image with the data in the given
	 * Buffer at the time the command is actually executed, which can be issued
	 * in a sequence. They must be created on the same context and match in size.
	 * 
	 * @param src Buffer to copy from
	 * @param dst Image to update
	 * @return CommandHandle Command for updating the Image
	 */
	[[nodiscard]] MINERVA_API CommandHandle updateImage(const BufferImp& src, const Image& dst);
	/**
	 * @brief Updates the given Tensor using the given buffer.
	 * @ingroup memory
	 * 
	 * Creates a command for updating the given Tensor with the data in the given
	 * Buffer at the time the command is actually executed, which can be issued
	 * in a sequence. They must be created on the same context and match in size.
	 * 
	 * @param src Buffer to copy from
	 * @param dst Tensor to update
	 * @return CommandHandle Command for updating the Tensor
	 */
	[[nodiscard]] MINERVA_API CommandHandle updateTensor(const BufferImp& src, const TensorImp& dst);

}
