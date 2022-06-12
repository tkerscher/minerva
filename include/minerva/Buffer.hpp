#pragma once

#include "minerva/Handles.hpp"
#include "minerva/span.hpp"
#include "minerva/config.hpp"

namespace minerva {
	/**
	 * @brief Base class for Buffer holding the implementation.
	 * @ingroup memory
	 */
	class MINERVA_API BufferImp {
	public:
		/**
		 * @brief Returns a view on the memory hold by the buffer.
		 * 
		 * @return span<std::byte> view on the memory
		 */
		[[nodiscard]] span<std::byte> getMemory() const;
		/**
		 * @brief Returns the size of the buffer in bytes.
		 * 
		 * @return uint64_t size of the buffer in bytes.
		 */
		[[nodiscard]] uint64_t size_bytes() const;

		BufferImp(BufferImp&& other) noexcept;
		BufferImp& operator=(BufferImp&& other) noexcept;

		/**
		 * @brief Construct a new Buffer
		 * 
		 * @param context context on which to create the buffer
		 * @param size size of the buffer in bytes
		 */
		BufferImp(const ContextHandle& context, uint64_t size);
		/**
		 * @brief Construct a new Buffer
		 * 
		 * @param context context on which to create the buffer
		 * @param data data to initialize the buffer with
		 */
		BufferImp(const ContextHandle& context, span<const std::byte> data);
		virtual ~BufferImp();
		
	public:
		/// @private
		[[nodiscard]] const vulkan::Buffer& getBuffer() const;

	private:
		BufferHandle buffer;
		span<std::byte> memory;
	};

	/**
	 * @brief Buffer managing memory hosted by the CPU presented as an array of type T.
	 * 
	 * @tparam T element type of the array
	 */
	template<class T>
	class Buffer : public BufferImp {
	public:
		/**
		 * @brief Returns a view on the memory hold by the buffer.
		 * 
		 * @return span<T> view on the memory
		 */
		[[nodiscard]]
		span<T> getMemory() const {
			auto mem = BufferImp::getMemory();
			return { 
				reinterpret_cast<T*>(mem.data()),
				mem.size() / sizeof(T)
			};
		}

		/**
		 * @brief Returns the number of elements in the managed array
		 * 
		 * @return size_t number of elements
		 */
		[[nodiscard]]
		size_t size() const {
			return size_bytes() / sizeof(T);
		}

		/**
		 * @brief Construct a new Buffer object
		 * 
		 * @param context context on which to create this
		 * @param count number of elements in array
		 */
		Buffer(const ContextHandle& context, size_t count)
			: BufferImp(context, sizeof(T) * count)
		{}
		/**
		 * @brief Construct a new Buffer object
		 * 
		 * @param context context on which to create this
		 * @param data data to initialize the buffer with
		 */
		Buffer(const ContextHandle& context, span<const T> data)
			: BufferImp(context, as_bytes(data))
		{}
		/**
		 * @brief Construct a new Buffer object
		 * 
		 * @param context context on which to create this
		 * @param init data to initialize the buffer with
		 */
		Buffer(const ContextHandle& context, std::initializer_list<T> init)
			: Buffer(context, span<const T>{init})
		{}
	};

	template<class Container> Buffer(const ContextHandle&, const Container&)->Buffer<typename Container::value_type>;
}

/**
 * @class minerva::Buffer
 * @ingroup memory
 * 
 * Buffer manages data living on the CPU side, which can be used to initialize
 * Tensor and Image objects or retrieve data from them.
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
 * @see Image
 * @see Tensor
 */
