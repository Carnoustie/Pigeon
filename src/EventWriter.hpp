#include <cstdio>
#include <ctime>
#include "Event.hpp"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "TCPsocket.hpp"

class EventWriter{
	private:
	public:
		ClientTCPsocket connection;

		EventWriter(const char* targetHost, const char* targetPort){
			connection = ClientTCPsocket(targetHost, targetPort);
		}

		virtual void writeEvent(Event& e){
			int btsSent = sendto(connection.getSockFd(), &e, sizeof(Event), 0, connection.getTargetAddr()->ai_addr, connection.getTargetAddr()->ai_addrlen );
		}

		void announceEventCategories(int numEventCategories, EventCategory* EventCategories){
			int btsSent = sendto(connection.getSockFd(), &numEventCategories, sizeof(int), 0, connection.getTargetAddr()->ai_addr, connection.getTargetAddr()->ai_addrlen);
			btsSent = sendto(connection.getSockFd(), EventCategories, sizeof(EventCategory[numEventCategories]), 0, connection.getTargetAddr()->ai_addr, connection.getTargetAddr()->ai_addrlen);
		}
};
