#pragma once

#include "minerva/Buffer.hpp"
#include "minerva/Handles.hpp"
#include "minerva/Tensor.hpp"

namespace minerva {
	class MINERVA_API CopyCommandFactory {
	public:
		[[nodiscard]]
		static CommandHandle retrieveTensor(const TensorImp& src, const BufferImp& dst);
		[[nodiscard]]
		static CommandHandle updateTensor(const BufferImp& src, const TensorImp& dst);

		CopyCommandFactory() = delete;
	};

	//syntactic sugar
	template<class Src, class Dst>
	[[nodiscard]] inline CommandHandle retrieveTensor(
		const Tensor<Src>& src, const Buffer<Dst>& dst)
	{
		return CopyCommandFactory::retrieveTensor(src, dst);
	}
	template<class Src, class Dst>
	[[nodiscard]] inline CommandHandle updateTensor(
		const Buffer<Src>& src, const Tensor<Dst>& dst)
	{
		return CopyCommandFactory::updateTensor(src, dst);
	}
}
