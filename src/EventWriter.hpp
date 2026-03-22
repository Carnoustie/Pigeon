
#include <chrono>
#include <cstdint>
#include <ctime>
#include "Event.hpp"

class EventWriter{
	public:
		virtual void writeEvent(Event e) =0;
};
