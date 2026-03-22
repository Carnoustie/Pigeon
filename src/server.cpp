#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <random>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "EventWriter.hpp"

#define BACKLOG 10
	


int main(int argc, char* argv[]){

	printf("\n\n\nGoldman in 18 months\n\n");

	addrinfo hints;
	addrinfo* result;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	
	if(getaddrinfo(NULL, "8080", &hints, &result)!=0){
		printf("\n\nFailed getting address!");
	}

	int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if(bind(sockfd, result->ai_addr, result->ai_addrlen)!=0){
		printf("\n\nFailed binding!");
	}

	if(listen(sockfd,BACKLOG)!=0){
		printf("\n\nFailed listening!");
	}

	struct sockaddr_storage clientAddr;
	socklen_t clientAddrLen = sizeof clientAddr;
	
	int connFD = accept(sockfd, (struct sockaddr*) &clientAddr, &clientAddrLen);

	printf("\n\nClient connected!");

	Event buff[1];

	while(1){
		int readBytes = recvfrom(connFD, buff, sizeof(Event), 0, (sockaddr*) &clientAddr, &clientAddrLen);
		printf("\n\n\nReceived: %s\n\n", buff[0].logMessage );
	}


	

}
