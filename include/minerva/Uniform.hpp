#pragma once

#include "minerva/Handles.hpp"
#include "minerva/span.hpp"
#include "minerva/config.hpp"

namespace minerva {
	/**
	 * @brief Base class for Uniform holding the implementation
	 * @ingroup memory
	 */
	class MINERVA_API UniformImp {
	public:
		/**
		 * @brief Returns a pointer to the memory hold by this uniform.
		 */
		[[nodiscard]] void* getData() const noexcept;

		UniformImp(UniformImp&& other) noexcept;
		UniformImp& operator=(UniformImp&& other) noexcept;

		/**
		 * @brief Construct a new Uniform Imp object
		 * 
		 * @param context context on which to create this
		 * @param size size in bytes
		 */
		UniformImp(const ContextHandle& context, uint64_t size);
		/**
		 * @brief Construct a new Uniform Imp object
		 * 
		 * @param context context on which to create this
		 * @param data data to initialize this with
		 */
		UniformImp(const ContextHandle& context, span<const std::byte> data);
		virtual ~UniformImp();

	public: //internal
		/// @private
		[[nodiscard]] const vulkan::Buffer& getBuffer() const noexcept;

	private:
		BufferHandle buffer;
	};

	/**
	 * @brief Uniform managing memory readable by the GPU and writable by the CPU.
	 * 
	 * @tparam T type of the data.
	 */
	template<class T>
	class Uniform : public UniformImp {
	public:
		T* operator->() const noexcept {
			return reinterpret_cast<T*>(getData());
		}
		T& operator*() const noexcept {
			return *reinterpret_cast<T*>(getData());
		}

		/**
		 * @brief Construct a new Uniform
		 * 
		 * @param context context in which to create this
		 */
		Uniform(const ContextHandle& context)
			: UniformImp(context, sizeof(T))
		{}
		/**
		 * @brief Construct a new Uniform
		 * 
		 * @param context context in which to create this
		 * @param value value to initialize the Uniform with
		 */
		Uniform(const ContextHandle& context, const T& value)
			: UniformImp(context, { reinterpret_cast<const std::byte*>(&value), sizeof(T)})
		{}
	};
}

/**
 * @class minerva::Uniform
 * @ingroup memory
 * 
 * Uniform manages a uniform buffer object on the GPU, which is mapped on the
 * CPU side to allow both reads from the GPU and updates from the CPU. UBOs can
 * be defined in the shader like the following:
 * 
 * @code {.unparsed}
 * layout(std140, binding = 0) uniform UBO {
 * 		float someFloat;
 * 		int someInt;
 * 		float anotherFloat;
 * } ubo;
 * @endcode
 * 
 * Interacting with this UBO can look like this:
 * 
 * @code {.cpp}
 * struct UBO {
 * 		float someFloat;
 * 		int someInt;
 * 		float anotherFloat;
 * };
 * 
 * Uniform<UBO> ubo(context);
 * ubo->someInt = 4;
 * @endcode
 */
