
#include <netdb.h>
#include <pthread.h>
#include <unordered_map>
#include "TCPsocket.hpp"

struct conn{
	int connSockFd;
	addrinfo* connAddr;
	pthread_t connThread;
	int connId;
};

class EventBroker{
	private:
		int maxEventWriterConnections;
		int maxEventReaderConnections;
		ServerTCPsocket evtWrtrSock;
		ServerTCPsocket evtRdrSock;
		const char* evtWrtrsPort = "8080";
		const char* evtRdrsPort = "8081";
		conn* wrtrConns;
		conn* rdrConns;
	
	public:
		EventBroker(int maxWriters, int maxReaders){
			maxEventWriterConnections = maxWriters;
			maxEventReaderConnections = maxReaders;
			evtWrtrSock = ServerTCPsocket(evtWrtrsPort, maxWriters);
			evtRdrSock = ServerTCPsocket(evtRdrsPort, maxReaders);
			wrtrConns = new conn[maxWriters];
			rdrConns = new conn[maxReaders];
		}
	
};
