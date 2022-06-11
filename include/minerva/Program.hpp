#pragma once

#include <vector>

#include "minerva/Handles.hpp"
#include "minerva/Image.hpp"
#include "minerva/Tensor.hpp"
#include "minerva/Uniform.hpp"
#include "minerva/config.hpp"
#include "minerva/span.hpp"

namespace minerva {
	template<class T>
	struct Dispatch;

	template<>
	struct Dispatch<void> {
		uint32_t groupCountX;
		uint32_t groupCountY;
		uint32_t groupCountZ;

		span<const std::byte> push;

		constexpr Dispatch(uint32_t x, uint32_t y, uint32_t z, span<const std::byte> push)
			: groupCountX(x), groupCountY(y), groupCountZ(z), push(push)
		{}

		constexpr Dispatch(uint32_t x) : Dispatch(x, 1, 1, {}) {}
		constexpr Dispatch(uint32_t x, uint32_t y) : Dispatch(x, y, 1, {}) {}
		constexpr Dispatch(uint32_t x, uint32_t y, uint32_t z) : Dispatch(x, y, z, {}) {}
		
		constexpr Dispatch(uint32_t x, span<const std::byte> push) : Dispatch(x, 1, 1, push) {}
		constexpr Dispatch(uint32_t x, uint32_t y, span<const std::byte> push) : Dispatch(x, y, 1, push) {}
	};

	template<class T>
	struct Dispatch : public Dispatch<void> {
		T pushValue;

		constexpr Dispatch(uint32_t x, uint32_t y, uint32_t z, T value)
			: Dispatch<void>(x, y, z, { reinterpret_cast<const std::byte*>(&pushValue), sizeof(T) })
			, pushValue(std::move(value))
		{}
		constexpr Dispatch(uint32_t x, uint32_t y, T value)
			: Dispatch<void>(x, y, 1, { reinterpret_cast<const std::byte*>(&pushValue), sizeof(T) })
			, pushValue(std::move(value))
		{}
		constexpr Dispatch(uint32_t x, T value)
			: Dispatch<void>(x, 1, 1, { reinterpret_cast<const std::byte*>(&pushValue), sizeof(T) })
			, pushValue(std::move(value))
		{}
	};

	class MINERVA_API Parameter {	
	public:
		void setArgument(const Image& img, uint32_t binding = 0);
		void setArgument(const TensorImp& tensor, uint32_t binding = 0);
		void setArgument(const UniformImp& uniform, uint32_t binding = 0);

		~Parameter();
	private:
		struct pImpl;
		std::unique_ptr<pImpl> _pImpl;

		Parameter(std::unique_ptr<pImpl> pImpl);

		friend class Program;
	};

	class MINERVA_API Program {
	private:
		//Internal steps
		void beginCommand();
		void bindParams(span<const Parameter> params);
		void dispatch(const Dispatch<void>& dispatch);
		CommandHandle endCommand();

	public:
		[[nodiscard]] Parameter createParameter(uint32_t set = 0) const;
		[[nodiscard]] uint32_t getParameterCount() const;

		template<class T = void>
		[[nodiscard]] CommandHandle Run(const Dispatch<T>& batch) {
			beginCommand();
			dispatch(batch);
			return endCommand();
		}

		template<class T = void>
		[[nodiscard]] CommandHandle Run(const Dispatch<T>& batch, const Parameter& param) {
			beginCommand();
			bindParams({ &param, 1 });
			dispatch(batch);
			return endCommand();
		}

		template<class T = void>
		[[nodiscard]] CommandHandle Run(const Dispatch<T>& batch, span<const Parameter> params) {
			beginCommand();
			bindParams(params);
			dispatch(batch);
			return endCommand();
		}

		template<class T = void>
		[[nodiscard]] CommandHandle Run(span<const Dispatch<T>> batch) {
			beginCommand();
			for (auto& d : batch)
				dispatch(d);
			return endCommand();
		}

		template<class T = void>
		[[nodiscard]] CommandHandle Run(span<const Dispatch<T>> batch, const Parameter& param) {
			beginCommand();
			bindParams({ &param, 1 });
			for (auto& d : batch)
				dispatch(d);
			return endCommand();
		}

		template<class T = void>
		[[nodiscard]] CommandHandle Run(span<const Dispatch<T>> batch, span<const Parameter> params) {
			beginCommand();
			bindParams(params);
			for (auto& d : batch)
				dispatch(d);
			return endCommand();
		}

		Program(const ContextHandle& context, span<const uint32_t> code);
		Program(const ContextHandle& context, const char* filename);

		Program(const ContextHandle& context, span<const uint32_t> code, span<const std::byte> consts);
		Program(const ContextHandle& context, const char* filename, span<const std::byte> consts);
		
		template<class T>
		Program(const ContextHandle& context, span<const uint32_t> code, const T& consts)
			: Program(context, code, { reinterpret_cast<const std::byte*>(&consts), sizeof(T) })
		{}
		template<class T>
		Program(const ContextHandle & context, const char* filename, const T & consts)
			: Program(context, filename, { reinterpret_cast<const std::byte*>(&consts), sizeof(T) })
		{}

		~Program();

	private:
		struct pImpl;
		std::unique_ptr<pImpl> _pImpl;
	};

	[[nodiscard]] std::vector<uint32_t> MINERVA_API loadShader(const char* filename);
}
