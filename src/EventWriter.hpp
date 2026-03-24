#include <chrono>
#include <cstdint>
#include <ctime>
#include "Event.hpp"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

class EventWriter{
	private:
	public:
		virtual void writeEvent(Event e) = 0;
		EventWriter(){
			
		}
};
