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
			int btsSent = recvfrom(connection.sockFd, &e, sizeof(Event), 0, connection.targetAddr->ai_addr, &connection.targetAddr->ai_addrlen );
			printf("\n\nEventReader just read event: %s", e.logMessage);
		}

		void subscribeToEventCategories(int numEventCategories, EventCategory EventCategories[10]){
			int btsSent = sendto(connection.sockFd, &numEventCategories, sizeof(int), 0, connection.targetAddr->ai_addr, connection.targetAddr->ai_addrlen);
			btsSent = sendto(connection.sockFd, EventCategories, sizeof(EventCategory[numEventCategories]), 0, connection.targetAddr->ai_addr, connection.targetAddr->ai_addrlen);
		}
};
