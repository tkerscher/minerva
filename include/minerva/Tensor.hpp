#pragma once

#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API TensorImp {
	public:

		[[nodiscard]] uint64_t size_bytes() const;

		TensorImp(const ContextHandle& context, uint64_t size);
		virtual ~TensorImp();

	public: //internal
		[[nodiscard]] const vulkan::Buffer& getBuffer() const;

	private:
		BufferHandle buffer;
		uint64_t _size;
	};

	template<class T>
	class Tensor : public TensorImp {
	public:
		[[nodiscard]]
		inline size_t size() const {
			return size_bytes() / sizeof(T);
		}

		Tensor(const ContextHandle& context, size_t count)
			: TensorImp(context, sizeof(T) * count)
		{}
	};

}
