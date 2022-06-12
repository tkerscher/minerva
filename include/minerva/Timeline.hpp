#pragma once

#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	/**
	 * @brief Class used for controlling time dependency on commands. 
	 */
	class MINERVA_API Timeline {
	public:
		/**
		 * @brief Returns the last finished step.
		 * 
		 * @return uint64_t The last finished step.
		 */
		[[nodiscard]] uint64_t getValue();
		/**
		 * @brief Sets the last finished step.
		 * 
		 * Updates the last finished step as returned by \ref Timeline::getValue().
		 * Can be used to notify other observer of the timeline  such as commands
		 * issued on the GPU or other tasks after a step on the CPU side finished. 
		 * 
		 * @param value The last finished step. Must be at least the current value.
		 */
		void setValue(uint64_t value);

		/**
		 * @brief Waits for the timeline to reach the given step.
		 * 
		 * Blocks the calling thread until timeline reaches the given step.
		 * Returns immediately if the value is at least the given one.
		 * Note that if the given value is never reached, a deadlock is created.
		 * 
		 * @param value The step to wait for.
		 */
		void waitValue(uint64_t value) const;
		/**
		 * @brief Waits for the timeline to reach the given step for a certain time.
		 * 
		 * Blocks the calling thread until timeline reaches the given step or
		 * timeout nanoseconds elapsed. Returns true in the former and false in
		 * the latter case. Returns immediately true if the value is at least
		 * the given one. 
		 * 
		 * @param value The step to wait for.
		 * @param timeout Time in nanoseconds to wait for.
		 * @return true Timeline reached at least the given step in timeout nanoseconds.
		 * @return false Timeout ran out.
		 */
		bool waitValue(uint64_t value, uint64_t timeout) const;

		/**
		 * @brief Construct a new Timeline object
		 * 
		 * @param context Context in which this will be created.
		 * @param initialValue The step this timeline starts in.
		 */
		Timeline(const ContextHandle& context, uint64_t initialValue = 0);

	public:
		/// @private
		TimelineHandle timeline;
	};

	//TODO: waitAll, waitAny for span<Timeline>
}

/**
 * @class minerva::Timeline
 * @ingroup command
 * 
 * minerva::Timeline holds a strictly increasing value for the last finished
 * step, which can be used to coordinate between GPU-GPU, GPU-CPU and CPU-CPU.
 * Allows to query the progress of a sequence or for the GPU to wait on the CPU.
 * 
 * @code {.cpp}
 * Timeline timeline(context);
 * 
 * // *** some gpu code ***
 * 
 * //wait for the gpu
 * timeline.waitValue(5);
 * //do some stuff on the cpu
 * //notify the gpu to carry on
 * timeline.setValue(6);
 * 
 * while(!timeline.waitValue(10,20000))
 *     std::cout << "Still runnning...\n";
 * 
 * std::cout << "Done!\n";
 * @endcode
 * 
 * 
 * @see minerva::SequenceBuilder
 */
