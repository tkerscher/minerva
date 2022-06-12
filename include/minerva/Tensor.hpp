#pragma once

#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	/**
	 * @brief Base class for Tensor holding the implementation.
	 * @ingroup memory
	 */
	class MINERVA_API TensorImp {
	public:
		/**
		 * @brief Returns the size of the tensor.
		 * 
		 * @return uint64_t Size of the tensor in bytes.
		 */
		[[nodiscard]] uint64_t size_bytes() const;

		/**
		 * @brief Construct a new Tensor Imp object
		 * 
		 * @param context Context in which to create this
		 * @param size Size in bytes.
		 */
		TensorImp(const ContextHandle& context, uint64_t size);
		virtual ~TensorImp();

	public: //internal
		/// @private
		[[nodiscard]] const vulkan::Buffer& getBuffer() const;

	private:
		BufferHandle buffer;
		uint64_t _size;
	};

	/**
	 * @brief Tensor managing memory on the GPU presented as an array of type T.
	 * 
	 * @tparam T element type of the array.
	 */
	template<class T>
	class Tensor : public TensorImp {
	public:
		/**
		 * @brief Returns the number of elements in this Tensor.
		 * 
		 * @return size_t Number of elements.
		 */
		[[nodiscard]]
		inline size_t size() const {
			return size_bytes() / sizeof(T);
		}

		/**
		 * @brief Construct a new Tensor object
		 * 
		 * @param context context in which to create this
		 * @param count number of elements in the array.
		 */
		Tensor(const ContextHandle& context, size_t count)
			: TensorImp(context, sizeof(T) * count)
		{}
	};
}

/**
 * @class minerva::Tensor
 * @ingroup memory
 * 
 * Tensor manages a storage buffer object on the GPU. In the shader
 * these can be defined like in the following:
 * 
 * @code {.unparsed}
 * layout(std430, binding = 0) buffer tensor { float x[]; };
 * @endcode
 * 
 * The benefit here is, that the shader does not need to know the actual size of
 * the SBO. This should be used for large data.
 * 
 * To fill the tensor you need to copy it from a Buffer using a command
 * issued to a sequence. Likewise, to retrieve the data, you also need a command:
 * 
 * @code {.cpp}
 * Buffer<float> buffer(context, {1.f, 2.f, 3.f, 4.f});
 * Tensor<float> tensor(context, 4);
 * 
 * auto copyTo = updateTensor(buffer, tensor);
 * auto copyFrom = retrieveTensor(tensor, buffer);
 * 
 * Timeline timeline(context);
 * beginSequence(timeline)
 * 		.And(copyTo)
 * 		.Then(copyFrom)
 * 		.Submit();
 * @endcode
 * 
 * @see Buffer
 * @see updateTensor
 * @see retrieveTensor
 */
