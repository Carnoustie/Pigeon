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
			int btsSent = send(connection.getSockFd(), &e, sizeof(Event), 0);
		}

		void announceEventCategories(int numEventCategories, EventCategory* EventCategories){
			int btsSent = send(connection.getSockFd(), &numEventCategories, sizeof(int), 0);
			btsSent = send(connection.getSockFd(), EventCategories, sizeof(EventCategory[numEventCategories]), 0);
		}
};
