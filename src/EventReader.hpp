#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include "Event.hpp"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "TCPsocket.hpp"

class EventReader{
	private:
	public:
		ClientTCPsocket connection;

		EventReader(const char* targetHost, const char* targetPort){
			connection = ClientTCPsocket(targetHost, targetPort);
		}

		virtual void readEvent(){
			Event e;
			int btsSent = recv(connection.getSockFd(), &e, sizeof(Event), 0);
			printf("\n\nEventReader just read event: %s", e.logMessage);
		}

		void subscribeToEventCategories(int numEventCategories, EventCategory EventCategories[10]){
			int btsSent = send(connection.getSockFd(), &numEventCategories, sizeof(int), 0);
			btsSent = send(connection.getSockFd(), EventCategories, sizeof(EventCategory[numEventCategories]), 0);
		}
};
