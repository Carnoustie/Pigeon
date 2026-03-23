#include "Event.hpp"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


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


void playTradingInterval(int sockFD, addrinfo* source, int intervalInSeconds){

	Event fullEvent;
	FTP tradingEvent;

	while(1){
		int readBytes = recvfrom(sockFD, &fullEvent, sizeof(Event), 0, source->ai_addr, &source->ai_addrlen);
		printf("\n\nEventReader read: %s", fullEvent.logMessage);
	}
}


int main(int argc, char* argv[]){

	struct addrinfo hints;
	struct addrinfo* result;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET6;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo("localhost", "8080", &hints, &result)!=0){
		printf("\n\ngetting address failed due to %s", strerror(errno));
	}

	int sockFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if(connect(sockFD, result->ai_addr, result->ai_addrlen)!=0){
		printf("\n\nconnecting failed due to %s", strerror(errno));
	}

	playTradingInterval(sockFD, result, 10);


}
