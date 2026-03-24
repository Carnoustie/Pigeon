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
#include "EventBroker.hpp"

#define BACKLOG 10
	

#define NUMEVENTQUEUES 5

int main(int argc, char* argv[]){

	printf("\n\n\nGoldman in 18 months\n\n");


	EventBroker evtBrkr = EventBroker(1,1);

	evtBrkr.AcceptEventWriters();
	evtBrkr.AcceptEventReaders();

	pthread_join(evtBrkr.evtWrtrsBdry.rootThread,NULL);
	pthread_join(evtBrkr.evtRdrsBdry.rootThread, NULL);

	// pthread_join(tid1, NULL);
	// pthread_join(tid2, NULL);











	


	

}

