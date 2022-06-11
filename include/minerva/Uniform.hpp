#pragma once

#include "minerva/Handles.hpp"
#include "minerva/span.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API UniformImp {
	public:
		[[nodiscard]] void* getData() const noexcept;

		UniformImp(UniformImp&& other) noexcept;
		UniformImp& operator=(UniformImp&& other) noexcept;

		UniformImp(const ContextHandle& context, uint64_t size);
		UniformImp(const ContextHandle& context, span<const std::byte> data);
		virtual ~UniformImp();

	public:
		[[nodiscard]] const vulkan::Buffer& getBuffer() const noexcept;

	private:
		BufferHandle buffer;
	};

	template<class T>
	class Uniform : public UniformImp {
	public:
		T* operator->() const noexcept {
			return reinterpret_cast<T*>(getData());
		}
		T& operator*() const noexcept {
			return *reinterpret_cast<T*>(getData());
		}

		Uniform(const ContextHandle& context)
			: UniformImp(context, sizeof(T))
		{}
		Uniform(const ContextHandle& context, const T& value)
			: UniformImp(context, { reinterpret_cast<const std::byte*>(&value), sizeof(T)})
		{}
	};
}
