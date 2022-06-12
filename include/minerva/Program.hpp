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

	/**
	 * @brief Struct holding information for a single Program dispatch.
	 * @ingroup command
	 * @see Dispatch
	 */
	template<>
	struct Dispatch<void> {
		uint32_t groupCountX; //!< number of local workgroups to dispatch in the X dimension.
		uint32_t groupCountY; //!< number of local workgroups to dispatch in the Y dimension.
		uint32_t groupCountZ; //!< number of local workgroups to dispatch in the Z dimension.

		span<const std::byte> push; //!< Data to fill the push constant with

		/**
		 * @brief Creates a new Dispatch struct.
		 * 
		 * @param x workgroups in x direction.
		 * @param y workgroups in y direction.
		 * @param z workgroups in z direction.
		 * @param push memory view on the data for the push constant
		 */
		constexpr Dispatch(uint32_t x, uint32_t y, uint32_t z, span<const std::byte> push)
			: groupCountX(x), groupCountY(y), groupCountZ(z), push(push)
		{}

		constexpr Dispatch(uint32_t x) : Dispatch(x, 1, 1, {}) {}
		constexpr Dispatch(uint32_t x, uint32_t y) : Dispatch(x, y, 1, {}) {}
		constexpr Dispatch(uint32_t x, uint32_t y, uint32_t z) : Dispatch(x, y, z, {}) {}
		
		constexpr Dispatch(uint32_t x, span<const std::byte> push) : Dispatch(x, 1, 1, push) {}
		constexpr Dispatch(uint32_t x, uint32_t y, span<const std::byte> push) : Dispatch(x, y, 1, push) {}
	};

	/**
	 * @brief Struct holding information for a single Program dispatch.
	 * 
	 * @tparam T type of the push constant, void if there is no push constant.
	 */
	template<class T>
	struct Dispatch : public Dispatch<void> {
		T pushValue; //!< The value of the push constant

		/**
		 * @brief Creates a new Dispatch struct.
		 * 
		 * @param x workgroups in x direction.
		 * @param y workgroups in y direction.
		 * @param z workgroups in z direction.
		 * @param value value of the push constant
		 */
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

	/**
	 * @brief Parameter holds a set of arguments used by a Program.
	 */
	class MINERVA_API Parameter {	
	public:
		/**
		 * @brief Sets the given Image as argument at the given binding.
		 * 
		 * @param img Image to bind
		 * @param binding where to bind
		 */
		void setArgument(const Image& img, uint32_t binding = 0);
		/**
		 * @brief Set the given Tensor as argument at the given binding.
		 * 
		 * @param tensor Tensor to bind
		 * @param binding where to bind
		 */
		void setArgument(const TensorImp& tensor, uint32_t binding = 0);
		/**
		 * @brief Set the given Uniform as argument at the given binding.
		 * 
		 * @param uniform Uniform to bind
		 * @param binding where to bind
		 */
		void setArgument(const UniformImp& uniform, uint32_t binding = 0);

		~Parameter();
	private:
		struct pImpl;
		std::unique_ptr<pImpl> _pImpl;

		Parameter(std::unique_ptr<pImpl> pImpl);

		friend class Program;
	};

	/**
	 * @brief Program holds a compute shader and allows dispatching it. 
	 */
	class MINERVA_API Program {
	private:
		//Internal steps
		void beginCommand();
		void bindParams(span<const Parameter> params);
		void dispatch(const Dispatch<void>& dispatch);
		CommandHandle endCommand();

	public:
		/**
		 * @brief Creates a Parameter of the given set
		 * 
		 * @param set set for which to create parameter
		 * @return Parameter the new created Parameter.
		 */
		[[nodiscard]] Parameter createParameter(uint32_t set = 0) const;
		/**
		 * @brief Returns the number of Parameter sets in this shader.
		 * 
		 * @return uint32_t number of Parameter sets.
		 */
		[[nodiscard]] uint32_t getParameterCount() const;

		/**
		 * @brief Issues a single dispatch of this shader
		 * 
		 * @tparam T Type of the push constant, void if there is none
		 * @param batch argument for the dispatch
		 * @return CommandHandle command with the dispatch recorded in
		 */
		template<class T = void>
		[[nodiscard]] CommandHandle Run(const Dispatch<T>& batch) {
			beginCommand();
			dispatch(batch);
			return endCommand();
		}

		/**
		 * @brief Issues a single dispatch of this shader with the given argument
		 * 
		 * @tparam T Type of the push constant, void if there is none
		 * @param batch argument for the dispatch
		 * @param param parameter passed to the shader
		 * @return CommandHandle command with the dispatch recorded in
		 */
		template<class T = void>
		[[nodiscard]] CommandHandle Run(const Dispatch<T>& batch, const Parameter& param) {
			beginCommand();
			bindParams({ &param, 1 });
			dispatch(batch);
			return endCommand();
		}

		/**
		 * @brief Issues a single dispatch of this shader with the given set of arguments
		 * 
		 * @tparam T Type of the push constant, void if there is none
		 * @param batch argument for the dispatch
		 * @param params sets of parameters passed to the shader
		 * @return CommandHandle command with the dispatch recorded in
		 */
		template<class T = void>
		[[nodiscard]] CommandHandle Run(const Dispatch<T>& batch, span<const Parameter> params) {
			beginCommand();
			bindParams(params);
			dispatch(batch);
			return endCommand();
		}

		/**
		 * @brief Issues multiple dispatches of the shader
		 * 
		 * @tparam T Type of the push constant, void if there is none 
		 * @param batch arguments for the dispatches
		 * @return CommandHandle command with the dispatches recorded in
		 */
		template<class T = void>
		[[nodiscard]] CommandHandle Run(span<const Dispatch<T>> batch) {
			beginCommand();
			for (auto& d : batch)
				dispatch(d);
			return endCommand();
		}

		/**
		 * @brief Issues multiple dispatches of the shader with the given param
		 * 
		 * @tparam T Type of the push constant, void if there is none 
		 * @param batch arguments for the dispatches 
		 * @param param parameter passed to the shader
		 * @return CommandHandle CommandHandle command with the dispatches recorded in
		 */
		template<class T = void>
		[[nodiscard]] CommandHandle Run(span<const Dispatch<T>> batch, const Parameter& param) {
			beginCommand();
			bindParams({ &param, 1 });
			for (auto& d : batch)
				dispatch(d);
			return endCommand();
		}

		/**
		 * @brief Issues multiple dispatches of the shader with the given set of params
		 * 
		 * @tparam T Type of the push constant, void if there is none 
		 * @param batch arguments for the dispatches 
		 * @param params sets of parameters passed to the shader
		 * @return CommandHandle CommandHandle command with the dispatches recorded in
		 */
		template<class T = void>
		[[nodiscard]] CommandHandle Run(span<const Dispatch<T>> batch, span<const Parameter> params) {
			beginCommand();
			bindParams(params);
			for (auto& d : batch)
				dispatch(d);
			return endCommand();
		}

		/**
		 * @brief Construct a new Program using the given code
		 * 
		 * @param context context on which to create this
		 * @param code view of memory holding the shader code
		 */
		Program(const ContextHandle& context, span<const uint32_t> code);
		/**
		 * @brief Construct a new Program using the given code
		 * 
		 * @param context context on which to create this
		 * @param filename path to file containing the shader code
		 */
		Program(const ContextHandle& context, const char* filename);

		/**
		 * @brief Construct a new Program using the given code and specializations.
		 * 
		 * @param context context on which to create this
		 * @param code view of memory holding the shader code
		 * @param consts view of memory holding the specializations
		 */
		Program(const ContextHandle& context, span<const uint32_t> code, span<const std::byte> consts);
		/**
		 * @brief Construct a new Program using the given code and specializations.
		 * 
		 * @param context context on which to create this
		 * @param filename path to file containing the shader code
		 * @param consts view of memory holding the specializations
		 */
		Program(const ContextHandle& context, const char* filename, span<const std::byte> consts);
		
		/**
		 * @brief Construct a new Program using the given code and specializations.
		 * 
		 * @tparam T type of specialization constants
		 * @param context context on which to create this
		 * @param code view of memory holding the shader code
		 * @param consts reference to struct holding the specialization consts
		 */
		template<class T>
		Program(const ContextHandle& context, span<const uint32_t> code, const T& consts)
			: Program(context, code, { reinterpret_cast<const std::byte*>(&consts), sizeof(T) })
		{}
		/**
		 * @brief Construct a new Program using the given code and specializations.
		 * 
		 * @tparam T type of specialization constants
		 * @param context context on which to create this
		 * @param filename path to file containing the shader code
		 * @param consts reference to struct holding the specialization consts
		 */
		template<class T>
		Program(const ContextHandle & context, const char* filename, const T & consts)
			: Program(context, filename, { reinterpret_cast<const std::byte*>(&consts), sizeof(T) })
		{}

		~Program();

	private:
		struct pImpl;
		std::unique_ptr<pImpl> _pImpl;
	};

	/**
	 * @brief loads the shader code from the given file
	 * @ingroup command
	 * 
	 * @param filename path to file
	 * @return std::vector<uint32_t> vector holding the shader code
	 */
	[[nodiscard]] std::vector<uint32_t> MINERVA_API loadShader(const char* filename);
}

/**
 * @class minerva::Dispatch
 * @ingroup command
 * 
 * Dispatch holds information for a single shader dispatch.
 * Compute shaders hold the following defining the size of a single work group:
 * 
 * @code {.unparsed}
 * layout(local_size_x = 8, local_size_y = 8) in;
 * @endcode
 * 
 * Dispatch now defines how many of these work groups in each dimension.
 * 
 * Additionally, one can also provide push constants. In the shader they look
 * like the following:
 * 
 * @code {.unparsed}
 * layout(push_constant) uniform constants {
 *   vec2 trans;
 *   float scale;
 * } push;
 * @endcode
 * 
 * and on the host side:
 * 
 * @code {.cpp}
 * struct Push {
 *	float transX;
 *	float transY;
 *	float scale;
 * };
 * Push push{};
 * Dispatch<Push> dispatch{ 16, 16, push };
 * @endcode
 * 
 * @see Program
 */

/**
 * @class minerva::Parameter
 * @ingroup command
 * 
 * Parameter holds a set of arguments, as defined in the shader by bindings
 * sharing the same set number:
 * 
 * @code {.unparsed}
 * layout(set = 0, binding = 0, rgba8) uniform readonly image2D inImage;
 * layout(set = 0, binding = 1, rgba8) uniform image2D outImage;
 * @endcode
 * 
 * Since one needs the shader to extract these informations, only Program can
 * create Parameter using Parameter::createParameter() with the set number given.
 * 
 * The bindings in a set can be bound to either Image, Tensor or Uniform using
 * Parameter::setArgument() with the binding number.
 * 
 * @see Program
 */

/**
 * @class minerva::Program
 * @ingroup command
 * 
 * Program holds a compute shader and allows to dispatch sets of runs.
 * 
 */
