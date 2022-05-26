#pragma once

#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
	class MINERVA_API Timeline {
	public:
		[[nodiscard]]
		uint64_t getValue();
		void setValue(uint64_t value);

		void waitValue(uint64_t value) const;
		bool waitValue(uint64_t value, uint64_t timeout) const;

		Timeline(const ContextHandle& context, uint64_t initialValue = 0);

	public:
		TimelineHandle timeline;
	};

	//TODO: waitAll, waitAny for span<Timeline>
}
