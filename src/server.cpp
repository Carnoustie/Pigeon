#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <queue>
#include <random>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "EventWriter.hpp"

#define BACKLOG 10
	

#define NUMEVENTQUEUES 5
std::queue<Event>* queues[NUMEVENTQUEUES];


struct connection{
	int socket;
	sockaddr_storage* clientAddr;
	socklen_t* clienatAddrLen;
};




void* handleEventWriter(void* arg){
	connection* conn = (connection*) arg;
	printf("\n\nClient connected!");

	Event buff[1];

	for(int i=0; i<NUMEVENTQUEUES;i++){
		queues[i] = new std::queue<Event>();
	}

	int queueIndex=0;
	while(1){
		int readBytes = recvfrom(conn->socket, buff, sizeof(Event), 0, (sockaddr*) conn->clientAddr, conn->clienatAddrLen);
		printf("\n\n\nReceived: %s\n\n", buff[0].logMessage );
		queues[queueIndex]->push(buff[0]);
		queueIndex++;
		queueIndex%=NUMEVENTQUEUES;
	}
}

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

	
	int connFD;



	struct sockaddr_storage eventWriterAddr;
	socklen_t eventWriterAddrLen = sizeof eventWriterAddr;
	
	connFD = accept(sockfd, (struct sockaddr*) &eventWriterAddr, &eventWriterAddrLen);

	connection writer = {
		connFD,
		&eventWriterAddr,
		&eventWriterAddrLen
	};

	pthread_t tid;
	pthread_create(&tid, NULL, handleEventWriter, &writer);

	struct sockaddr_storage eventReaderAddr;
	socklen_t eventReaderAddrLen = sizeof eventReaderAddr;
	
	connFD = accept(sockfd, (struct sockaddr*) &eventReaderAddr, &eventReaderAddrLen);

	connection reader = {
		connFD,
		&eventReaderAddr,
		&eventWriterAddrLen
	};

	//pthread_create(&tid, NULL, handleEventReader, &reader);

	

}

