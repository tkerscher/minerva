#pragma once

#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API TensorImp {
	public:
		TensorImp(const ContextHandle& context, uint64_t size);

	private:
		BufferHandle buffer;
	};

	template<class T>
	class Tensor : public TensorImp {
	public:
		Tensor(const ContextHandle& context, size_t count)
			: TensorImp(context, sizeof(T) * count)
		{}
	};

}
