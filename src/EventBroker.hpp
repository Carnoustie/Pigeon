#include <algorithm>
#include <cstring>
#include <memory>
#include <netdb.h>
#include <pthread.h>
#include <stdexcept>
#include <sys/socket.h>
#include "TCPsocket.hpp"
#include "Event.hpp"
#include <queue>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>

struct conn{
	std::queue<Event>* q;
	int qId;
	int sock;
	sockaddr_storage* clientAddr;
	socklen_t* clientAddrLen;
};

struct connHandle{
	std::unique_ptr<conn> connection;
	pthread_t connThread;
	int connId;
};

struct wrtrsPool{
	ServerTCPsocket TCPsock;
	std::vector<connHandle> connHandles;
	int maxEventWriterConnections;
	pthread_t rootThread;
	int connectedWriters;
	std::shared_ptr<std::vector<std::queue<Event>>> evtQueues;
	int numEvtQueues;
};

struct rdrsPool{
	ServerTCPsocket TCPsock;
	std::vector<connHandle> connHandles;
	int maxEventReaderConnections;
	pthread_t rootThread;
	int connectedReaders;
	std::shared_ptr<std::vector<std::queue<Event>>> evtQueues;
	int numEvtQueues;
};

class EventBroker{
	private:
		const char* evtWrtrsPort = "8080";
		const char* evtRdrsPort = "8081";
	
	public:
		wrtrsPool evtWrtrsBdry;
		rdrsPool evtRdrsBdry;
		int numEvtQueues;
		std::shared_ptr<std::vector<std::queue<Event>>> evtQueues;
		
		//default constructor
		EventBroker():
			evtWrtrsBdry{},
			evtRdrsBdry{},
			evtWrtrsPort{nullptr},
			evtRdrsPort{nullptr},
			numEvtQueues{0},
			evtQueues{}
		 {}

		//constructor
		EventBroker(int maxWriters, int maxReaders){
			numEvtQueues = std::max(maxWriters, maxReaders);
			evtQueues =  std::make_shared<std::vector<std::queue<Event>>>(std::vector<std::queue<Event>>(numEvtQueues));

			evtWrtrsBdry.TCPsock = ServerTCPsocket(evtWrtrsPort, maxWriters);
			evtWrtrsBdry.connHandles = std::vector<connHandle>(maxWriters);
			evtWrtrsBdry.maxEventWriterConnections = maxWriters;
			evtWrtrsBdry.connectedWriters = 0;
			evtWrtrsBdry.evtQueues = evtQueues;
			evtWrtrsBdry.numEvtQueues = numEvtQueues;
			
			evtRdrsBdry.TCPsock = ServerTCPsocket(evtRdrsPort, maxReaders);
			evtRdrsBdry.connHandles = std::vector<connHandle>(maxReaders);
			evtRdrsBdry.maxEventReaderConnections = maxReaders;
			evtRdrsBdry.connectedReaders = 0;
			evtRdrsBdry.evtQueues = evtQueues;
			evtRdrsBdry.numEvtQueues = numEvtQueues;
		}

		//destructor
		~EventBroker(){
			for(int i=0; i<evtWrtrsBdry.connHandles.size(); i++){
				pthread_join(evtWrtrsBdry.connHandles[i].connThread, NULL);
			}
			pthread_join(evtWrtrsBdry.rootThread, NULL);
			
			for(int i=0; i<evtRdrsBdry.connHandles.size(); i++){
				pthread_join(evtRdrsBdry.connHandles[i].connThread, NULL);
			}
			pthread_join(evtRdrsBdry.rootThread, NULL);
		}

		static void* handleEventWriter(void* arg){
			conn* c = (conn*) arg;
			printf("\n\nA new EventWriter connected!");

			Event e;
			int readBytes;
			while(1){
				readBytes = recvfrom(c->sock, &e, sizeof(Event), 0, (sockaddr*) c->clientAddr, c->clientAddrLen);
				printf("\n\n\nEventBroker received: %s\n\n", e.logMessage);
				c->q->push(e);
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
							&(*connPool->evtQueues)[connPool->connectedWriters],
							connPool->connectedWriters,
							connSockFd,
							&clientAddr,
							&clientAddrLen
						};
						pthread_t tid;
						std::unique_ptr<conn> cPtr{new conn{c}};
						pthread_create(&tid, NULL, handleEventWriter, cPtr.get());
						connHandle ch = {
							std::move(cPtr),
							tid,
							connPool->connectedWriters
						};
						connPool->connectedWriters++;
						connPool->connHandles.push_back(std::move(ch));
					}
				}
			}
		}

		void AcceptEventWriters(){
			pthread_create(&evtWrtrsBdry.rootThread, NULL, AccptEvtWriters, &evtWrtrsBdry);
		}

		static void* handleEventReader(void* arg){
			conn* c = (conn*) arg;

			printf("\n\nA new EventReader connected!");

			int bytesWritten;
			Event e;
			while(1){
				if(!c->q->empty()){
					e = c->q->front();
					c->q->pop();
					std::this_thread::sleep_for(std::chrono::seconds(1));
					bytesWritten = sendto(c->sock, &e, sizeof(Event), 0, (sockaddr*) c->clientAddr, *c->clientAddrLen);
				}
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
							&(*connPool->evtQueues)[connPool->connectedReaders],
							connPool->connectedReaders,
							connSockFd,
							&clientAddr,
							&clientAddrLen
						};
						pthread_t tid;
						std::unique_ptr<conn> cPtr{new conn{c}};
						pthread_create(&tid, NULL, handleEventReader, cPtr.get());
						connHandle ch = {
							std::move(cPtr),
							tid,
							connPool->connectedReaders
						};
						connPool->connectedReaders++;
						connPool->connHandles.push_back(std::move(ch));
					}
				}
			}
		}

		void AcceptEventReaders(){
			pthread_create(&evtRdrsBdry.rootThread, NULL, AccptEvtReaders, &evtRdrsBdry);
		}
};
