#pragma once

#include "minerva/Buffer.hpp"
#include "minerva/Image.hpp"
#include "minerva/Handles.hpp"
#include "minerva/Tensor.hpp"

namespace minerva {

	//retrieve

	[[nodiscard]] MINERVA_API CommandHandle retrieveImage(const Image& src, const BufferImp& dst);
	[[nodiscard]] MINERVA_API CommandHandle retrieveTensor(const TensorImp& src, const BufferImp& dst);

	//update

	[[nodiscard]] MINERVA_API CommandHandle updateImage(const BufferImp& src, const Image& dst);
	[[nodiscard]] MINERVA_API CommandHandle updateTensor(const BufferImp& src, const TensorImp& dst);

}
