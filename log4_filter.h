#pragma once
#include "log4cpp/Filter.hh"

namespace log4cpp
{
	class PriorityFilter : public Filter
	{
	public:
		PriorityFilter(Priority::PriorityLevel priority = Priority::NOTSET) :priority_(priority) {};
	protected:
		virtual Decision _decide(const LoggingEvent& event)
		{
			if (priority_ == Priority::NOTSET
				|| priority_ == event.priority) {
				return ACCEPT;
			}
			return DENY;
		}
	private:
		Priority::PriorityLevel priority_ = Priority::NOTSET;
	};
}