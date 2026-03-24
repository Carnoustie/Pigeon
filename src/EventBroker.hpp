#include <algorithm>
#include <cstring>
#include <netdb.h>
#include <pthread.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unordered_map>
#include "TCPsocket.hpp"
#include "Event.hpp"
#include <queue>
#include <chrono>
#include <thread>

struct conn{
	std::queue<Event>* q;
	int qId;
	int sock;
	sockaddr_storage* clientAddr;
	socklen_t* clientAddrLen;
};

struct connHandle{
	conn* connection;
	pthread_t connThread;
	int connId;
};

struct wrtrsPool{
	ServerTCPsocket TCPsock;
	connHandle* conns;
	int maxEventWriterConnections;
	pthread_t rootThread;
	int connectedWriters;
	std::queue<Event>* evtQueues;
	int numEvtQueues;
};

struct rdrsPool{
	ServerTCPsocket TCPsock;
	connHandle* conns;
	int maxEventReaderConnections;
	pthread_t rootThread;
	int connectedReaders;
	std::queue<Event>* evtQueues;
	int numEvtQueues;
};

class EventBroker{
	private:
		const char* evtWrtrsPort = "8080";
		const char* evtRdrsPort = "8081";
	
	public:
		wrtrsPool evtWrtrsBdry;
		wrtrsPool evtRdrsBdry;
		int numEvtQueues;
		std::queue<Event>* evtQueues;
		
		//default constructor
		EventBroker():
			evtWrtrsBdry{},
			evtRdrsBdry{},
			evtWrtrsPort{nullptr},
			evtRdrsPort{nullptr},
			numEvtQueues{0},
			evtQueues{nullptr}
		 {}

		//constructor
		EventBroker(int maxWriters, int maxReaders){
			numEvtQueues = std::max(maxWriters, maxReaders);
			evtQueues = new std::queue<Event>[numEvtQueues];
			
			evtWrtrsBdry.TCPsock = ServerTCPsocket(evtWrtrsPort, maxWriters);
			evtWrtrsBdry.conns = new connHandle[maxWriters];
			evtWrtrsBdry.maxEventWriterConnections = maxWriters;
			evtWrtrsBdry.connectedWriters = 0;
			evtWrtrsBdry.evtQueues = evtQueues;
			evtWrtrsBdry.numEvtQueues = numEvtQueues;
			
			evtRdrsBdry.TCPsock = ServerTCPsocket(evtRdrsPort, maxWriters);
			evtRdrsBdry.conns = new connHandle[maxWriters];
			evtRdrsBdry.maxEventWriterConnections = maxWriters;
			evtRdrsBdry.connectedWriters = 0;
			evtRdrsBdry.evtQueues = evtQueues;
			evtRdrsBdry.numEvtQueues = numEvtQueues;
		}

		static void* handleEventWriter(void* arg){
			conn* c = (conn*) arg;
			printf("\n\nEventWriter connected!");

			Event buff[1];

			int queueIndex=0;
			int readBytes;
			while(1){
				readBytes = recvfrom(c->sock, buff, sizeof(Event), 0, (sockaddr*) c->clientAddr, c->clientAddrLen);
				printf("\n\n\nEventBroker received: %s\n\n", buff[0].logMessage);
				c->q->push(buff[0]);
			}
		}

		static void* AccptEvtWriters(void* arg){
			wrtrsPool* connPool = (wrtrsPool*) arg;
			int connSockFd;
			while(1){
				while(connPool->connectedWriters<=connPool->maxEventWriterConnections){
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrLen = (socklen_t) sizeof(clientAddr);
					if((connSockFd=accept(connPool->TCPsock.sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen))<0){
						std::string msg = (std::string) strerror(errno);
						throw std::runtime_error(msg);
					}else{
						conn c =  {
							&(connPool->evtQueues[connPool->connectedWriters]),
							connPool->connectedWriters,
							connSockFd,
							&clientAddr,
							&clientAddrLen
						};
						pthread_t tid;
						pthread_create(&tid, NULL, handleEventWriter, &c);
						connHandle ch = {
							&c,
							tid,
							connPool->connectedWriters
						};
						connPool->connectedWriters++;
					}
				}
			}
		}

		void AcceptEventWriters(){
			pthread_create(&evtWrtrsBdry.rootThread, NULL, AccptEvtWriters, &evtWrtrsBdry);
		}

		static void* handleEventReader(void* arg){
			conn* c = (conn*) arg;

			printf("\n\nEventReader connected!");

			// Event eventBuffer[1];
			int bytesWritten;
			Event e;

			while(1){
				if(!c->q->empty()){
					e = c->q->front();
					c->q->pop();
					std::this_thread::sleep_for(std::chrono::seconds(1));
					bytesWritten = sendto(c->sock, &e, sizeof(Event), 0, (sockaddr*) c->clientAddr, *c->clientAddrLen);
				}


				//bytesWritten = sendto(conn->socket, &e, sizeof(e), 0, (sockaddr*) conn->clientAddr, *conn->clienatAddrLen);
			}
		}

		static void* AccptEvtReaders(void* arg){
			rdrsPool* connPool = (rdrsPool*) arg;
			int connSockFd;
			while(1){
				while(connPool->connectedReaders<=connPool->maxEventReaderConnections){
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrLen = (socklen_t) sizeof(clientAddr);
					if((connSockFd=accept(connPool->TCPsock.sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen))<0){
						std::string msg = (std::string) strerror(errno);
						throw std::runtime_error(msg);
					}else{
						conn c =  {
							&(connPool->evtQueues[connPool->connectedReaders]),
							connPool->connectedReaders,
							connSockFd,
							&clientAddr,
							&clientAddrLen
						};
						pthread_t tid;
						pthread_create(&tid, NULL, handleEventReader, &c);
						connHandle ch = {
							&c,
							tid,
							connPool->connectedReaders
						};
						connPool->connectedReaders++;
					}
				}
			}
		}

		void AcceptEventReaders(){
			if(pthread_create(&evtRdrsBdry.rootThread, NULL, AccptEvtReaders, &evtRdrsBdry)<0);
		}
};
