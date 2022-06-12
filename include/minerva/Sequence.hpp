#pragma once

#include "minerva/Handles.hpp"
#include "minerva/Timeline.hpp"
#include "minerva/config.hpp"

namespace minerva {
	/**
	 * @brief Builder class for creating a command sequence. 
	 */
	class MINERVA_API SequenceBuilder {
	public:
		/**
		 * @brief Issues the given command running parallel in the current step.
		 * 
		 * @param command The issued command.
		 * @return SequenceBuilder& This builder.
		 */
		SequenceBuilder& And(const CommandHandle& command);
		/**
		 * @brief Issues the given command to run after the previous step finished.
		 * 
		 * @param command The issued command.
		 * @return SequenceBuilder& This builder.
		 */
		SequenceBuilder& Then(const CommandHandle& command);
		/**
		 * @brief Issues the sequence to wait for the timeline to reach the given value before proceding.
		 * 
		 * @param value The step to wait for.
		 * @return SequenceBuilder& This builder.
		 */
		SequenceBuilder& WaitFor(uint64_t value);
		/**
		 * @brief Submits the sequence to the GPU.
		 * 
		 * @return uint64_t The step reached after the sequence finished.
		 */
		uint64_t Submit();

		/**
		 * @brief Construct a new Sequence Builder object
		 * 
		 * @param timeline Timeline used to coordinate sequence.
		 * @param startValue Step to wait before starting the sequence.
		 */
		SequenceBuilder(Timeline& timeline, uint64_t startValue = 0);
		~SequenceBuilder();
	private:
		struct pImp;
		std::unique_ptr<pImp> _pImp;
	};

	//syntactic sugar

	/**
	 * @brief Creates a minerva::SequenceBuilder and returns it.
	 * 
	 * @param timeline Timeline used to coordinate sequence.
	 * @param startValue Step to wait before starting the sequence.
	 * @return SequenceBuilder The created builder.
	 */
	[[nodiscard]]
	SequenceBuilder beginSequence(Timeline& timeline, uint64_t startValue = 0) {
		return SequenceBuilder(timeline, startValue);
	}
}

/**
 * @class minerva::SequenceBuilder
 * @ingroup command
 * 
 * minerva::SequenceBuilder allows to create a sequence of commands running
 * groups of commands after each other, while the commands in the group runs
 * in parallel. The groups can be build using minerva::SequenceBuilder::And
 * and a new group waiting on the previous one can be issued with
 * minerva::SequenceBuilder::Then or with minerva::SequenceBuilder::WaitFor one
 * that wait for an external set step.
 * 
 * @code {.cpp}
 * beginSequence(timeline)
 * 		.And(updateTensor)
 * 		.Then(processTensor)
 * 		.Then(retrieveTensor)
 * 		.Submit();
 * @endcode
 * 
 * @see minerva::Timeline
 */
