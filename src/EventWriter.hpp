#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include "Event.hpp"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "TCPsocket.hpp"

typedef char EventCategory[32];

typedef std::string EvtCat;

class EventWriter{
	private:
	public:
		ClientTCPsocket connection;

		EventWriter(const char* targetHost, const char* targetPort){
			connection = ClientTCPsocket(targetHost, targetPort);
		}

		virtual void writeEvent(Event& e){
			printf("\n\nsanity-checking message: %s", e.logMessage);
			int btsSent = sendto(connection.sockFd, &e, sizeof(Event), 0, connection.targetAddr->ai_addr, connection.targetAddr->ai_addrlen );
		}

		void announceEventCategories(int numEventCategories, EventCategory* EventCategories){
			std::cout << "\n\nSanity check cat: " << EventCategories[1];
			int btsSent = sendto(connection.sockFd, &numEventCategories, sizeof(int), 0, connection.targetAddr->ai_addr, connection.targetAddr->ai_addrlen);
			btsSent = sendto(connection.sockFd, EventCategories, sizeof(EventCategory[numEventCategories]), 0, connection.targetAddr->ai_addr, connection.targetAddr->ai_addrlen);
		}
};
