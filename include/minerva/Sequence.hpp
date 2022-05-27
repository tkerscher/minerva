#pragma once

#include "minerva/Handles.hpp"
#include "minerva/Timeline.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API SequenceBuilder {
	public:
		SequenceBuilder& And(const CommandHandle& command);
		SequenceBuilder& Then(const CommandHandle& command);
		SequenceBuilder& WaitFor(uint64_t value);
		uint64_t Submit();

		SequenceBuilder(Timeline& timeline, uint64_t startValue = 0);
		~SequenceBuilder();
	private:
		struct pImp;
		std::unique_ptr<pImp> _pImp;
	};

	//syntactic sugar
	[[nodiscard]]
	SequenceBuilder beginSequence(Timeline& timeline, uint64_t startValue = 0) {
		return SequenceBuilder(timeline, startValue);
	}
}
