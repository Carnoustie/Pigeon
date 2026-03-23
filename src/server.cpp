#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <queue>
#include <random>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
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
	printf("\n\nEventWriter connected!");

	Event buff[1];

	int queueIndex=0;
	int readBytes;
	while(1){
		readBytes = recvfrom(conn->socket, buff, sizeof(Event), 0, (sockaddr*) conn->clientAddr, conn->clienatAddrLen);
		printf("\n\n\nEventBroker received: %s\n\n", buff[0].logMessage);
		queues[queueIndex]->push(buff[0]);
		queueIndex++;
		queueIndex%=NUMEVENTQUEUES;
	}
}

void* handleEventReader(void* arg){
	connection* conn = (connection*) arg;

	printf("\n\nEventReader connected!");



	
	

	// Event eventBuffer[1];
	int bytesWritten;
	Event e;

	while(1){
		if(!queues[0]->empty()){
			e = queues[0]->front();
			queues[0]->pop();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			bytesWritten = sendto(conn->socket, &e, sizeof(Event), 0, (sockaddr*) conn->clientAddr, *conn->clienatAddrLen);
		}


		//bytesWritten = sendto(conn->socket, &e, sizeof(e), 0, (sockaddr*) conn->clientAddr, *conn->clienatAddrLen);
	}
}

int main(int argc, char* argv[]){

	printf("\n\n\nGoldman in 18 months\n\n");

	for(int i=0; i<NUMEVENTQUEUES;i++){
		queues[i] = new std::queue<Event>();
	}

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
	int connFD2 = accept(sockfd, (struct sockaddr*) &eventWriterAddr, &eventWriterAddrLen);

	connection writer = {
		connFD2,
		&eventWriterAddr,
		&eventWriterAddrLen
	};

	pthread_t tid1;
	pthread_create(&tid1, NULL, handleEventWriter, &writer);






	struct sockaddr_storage eventReaderAddr;
	socklen_t eventReaderAddrLen = sizeof eventReaderAddr;

	int connFD1 = accept(sockfd, (struct sockaddr*) &eventReaderAddr, &eventReaderAddrLen);

	connection reader = {
		connFD1,
		&eventReaderAddr,
		&eventReaderAddrLen
	};

	pthread_t tid2;
	pthread_create(&tid2, NULL, handleEventReader, &reader);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);











	


	

}

