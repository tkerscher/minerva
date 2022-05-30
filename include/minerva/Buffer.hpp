#pragma once

#include "minerva/Handles.hpp"
#include "minerva/span.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API BufferImp {
	public:
		[[nodiscard]] span<std::byte> getMemory() const;
		[[nodiscard]] uint64_t size_bytes() const;

		BufferImp(BufferImp&& other) noexcept;
		BufferImp& operator=(BufferImp&& other) noexcept;

		BufferImp(const ContextHandle& context, uint64_t size);
		BufferImp(const ContextHandle& context, span<const std::byte> data);
		virtual ~BufferImp();
		
	public:
		[[nodiscard]] const vulkan::Buffer& getBuffer() const;

	private:
		BufferHandle buffer;
		span<std::byte> memory;
	};

	template<class T>
	class Buffer : public BufferImp {
	public:
		[[nodiscard]]
		span<T> getMemory() const {
			auto mem = BufferImp::getMemory();
			return { 
				reinterpret_cast<T*>(mem.data()),
				mem.size() / sizeof(T)
			};
		}

		[[nodiscard]]
		size_t size() const {
			return size_bytes() / sizeof(T);
		}

		Buffer(const ContextHandle& context, size_t count)
			: BufferImp(context, sizeof(T) * count)
		{}
		Buffer(const ContextHandle& context, span<const T> data)
			: BufferImp(context, as_bytes(data))
		{}
		Buffer(const ContextHandle& context, std::initializer_list<T> init)
			: Buffer(context, span<const T>{init})
		{}
	};

	template<class Container> Buffer(const ContextHandle&, const Container&)->Buffer<typename Container::value_type>;
}
