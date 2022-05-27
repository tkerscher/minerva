#pragma once

#include "minerva/Handles.hpp"
#include "minerva/span.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API BufferImp {
	public:
		[[nodiscard]] span<std::byte> getMemory() const;
		[[nodiscard]] uint64_t size_bytes() const;

		BufferImp(const ContextHandle& context, uint64_t size);
		virtual ~BufferImp();
	private:
		BufferHandle buffer;
		span<std::byte> memory;
		friend class CopyCommandFactory;
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
	};
}
