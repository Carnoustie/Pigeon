#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "EventReader.hpp"

//Financial Trading Event Payload
struct FTP{
	char buyer[256];
	char seller[256];
	char ticker[256];
	int numUnits;
	double price;
};

//Financial Trading Event
struct FTE{
	uint64_t timestamp;
	char logMessage[4096];
	FTP payload;
};


typedef char EventCategory[32];

void playTradingInterval(EventReader& er, int intervalInSeconds){
	Event fullEvent;
	FTP tradingEvent;

	while(1){
		// int readBytes = recvfrom(er.connection.sockFd, &fullEvent, sizeof(Event), 0, er.connection.targetAddr->ai_addr , &er.connection.targetAddr->ai_addrlen);
		er.readEvent();
		printf("\n\nEventReader read: %s", fullEvent.logMessage);
	}
}

int main(int argc, char* argv[]){
	EventReader er("localhost", "8081");
	EventCategory ECS[3] =  {"FTE", "WE", "CBA"};
	er.subscribeToEventCategories(3, ECS);
	playTradingInterval(er, 10);
}
