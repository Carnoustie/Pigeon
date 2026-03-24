
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
	connHandle* conns;
	pthread_t rootThread;
};

struct rdrsPool{
	connHandle* conns;
	pthread_t rootThread;
};



class EventBroker{
	private:
		int maxEventWriterConnections;
		int maxEventReaderConnections;
		ServerTCPsocket evtWrtrSock;
		ServerTCPsocket evtRdrSock;
		const char* evtWrtrsPort = "8080";
		const char* evtRdrsPort = "8081";
		int numEvtQueues;
		std::queue<Event>* evtQueues;
	
	public:
		connHandle* wrtrConns;
		connHandle* rdrConns;
		int connectedWrtrs;
		int connectedRdrs;
		//default constructor
		EventBroker(): 
			maxEventWriterConnections{0}, 
			maxEventReaderConnections{0}, 
			evtWrtrSock{}, 
			evtRdrSock{}, 
			evtWrtrsPort{nullptr},
			evtRdrsPort{nullptr},
			wrtrConns{nullptr},
			rdrConns{nullptr},
			connectedWrtrs{0},
			connectedRdrs{0},
			numEvtQueues{0},
			evtQueues{nullptr}
		 {}

		//constructor
		EventBroker(int maxWriters, int maxReaders){
			maxEventWriterConnections = maxWriters;
			maxEventReaderConnections = maxReaders;
			evtWrtrSock = ServerTCPsocket(evtWrtrsPort, maxWriters);
			evtRdrSock = ServerTCPsocket(evtRdrsPort, maxReaders);
			wrtrConns = new connHandle[maxWriters];
			rdrConns = new connHandle[maxReaders];
			connectedWrtrs = 0;
			connectedRdrs = 0;
			numEvtQueues = std::max(maxEventWriterConnections, maxEventReaderConnections);
			evtQueues = new std::queue<Event>[numEvtQueues];
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

		void AccptEvtWriters(){
			int connSockFd;
			while(1){
				while(connectedWrtrs<=maxEventWriterConnections){
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrLen = (socklen_t) sizeof(clientAddr);
					if((connSockFd=accept(evtWrtrSock.sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen))<0){
						std::string msg = (std::string) strerror(errno);
						throw std::runtime_error(msg);
					}else{
						conn c =  {
							&evtQueues[connectedWrtrs],
							connectedWrtrs,
							connSockFd,
							&clientAddr,
							&clientAddrLen
						};
						pthread_t tid;
						pthread_create(&tid, NULL, handleEventWriter, &c);
						connHandle ch = {
							&c,
							tid,
							connectedWrtrs
						};
						connectedWrtrs++;
					}
				}
			}
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

		void AccptEvtReaders(){
			int connSockFd;
			while(1){
				while(connectedWrtrs<=maxEventReaderConnections){
					struct sockaddr_storage clientAddr;
					socklen_t clientAddrLen = (socklen_t) sizeof(clientAddr);
					if((connSockFd=accept(evtRdrSock.sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen))<0){
						std::string msg = (std::string) strerror(errno);
						throw std::runtime_error(msg);
					}else{
						conn c =  {
							&evtQueues[connectedRdrs],
							connectedRdrs,
							connSockFd,
							&clientAddr,
							&clientAddrLen
						};
						pthread_t tid;
						if(pthread_create(&tid, NULL, handleEventReader, &c)!=0){
							throw std::runtime_error("reader handler failed");
						}
						
						connHandle ch = {
							&c,
							tid,
							connectedRdrs
						};
						connectedRdrs++;
					}
				}
			}
		}
};
