#pragma once

#include <vector>

#include "minerva/Handles.hpp"
#include "minerva/Image.hpp"
#include "minerva/Tensor.hpp"
#include "minerva/config.hpp"
#include "minerva/span.hpp"

namespace minerva {
	class MINERVA_API Parameter {	
	public:
		void setArgument(const TensorImp& tensor, uint32_t binding = 0);
		void setArgument(const Image& img, uint32_t binding = 0);

		~Parameter();
	private:
		struct pImpl;
		std::unique_ptr<pImpl> _pImpl;

		Parameter(std::unique_ptr<pImpl> pImpl);

		friend class Program;
	};

	class MINERVA_API Program {
	public:
		[[nodiscard]] Parameter createParameter(uint32_t set = 0) const;
		[[nodiscard]] uint32_t getParameterCount() const;

		[[nodiscard]] CommandHandle Run(uint32_t x, uint32_t y, uint32_t z);
		[[nodiscard]] CommandHandle Run(uint32_t x, uint32_t y, uint32_t z, const Parameter& param);
		[[nodiscard]] CommandHandle Run(uint32_t x, uint32_t y, uint32_t z, span<const Parameter> params);

		Program(const ContextHandle& context, span<uint32_t> code);
		~Program();

	private:
		struct pImpl;
		std::unique_ptr<pImpl> _pImpl;
	};

	[[nodiscard]] std::vector<uint32_t> MINERVA_API loadShader(const char* filename);
}
