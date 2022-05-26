#pragma once

#include "minerva/Handles.hpp"
#include "minerva/span.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API BufferImp {
	public:
		[[nodiscard]] span<std::byte> getMemory() const;

		BufferImp(const ContextHandle& context, uint64_t size);

	private:
		BufferHandle buffer;
		span<std::byte> memory;
	};

	template<class T>
	class Buffer : private BufferImp {
	public:
		[[nodiscard]]
		span<T> getMemory() const {
			return BufferImp::getMemory();
		}

		Buffer(const ContextHandle& context, size_t count)
			: BufferImp(context, sizeof(T) * count)
		{}
	};
}
