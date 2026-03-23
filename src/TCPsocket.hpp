#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class TCPsocket{

	private:
		addrinfo* targetAddr;
		int sockFd;
	public:
		TCPsocket(const TCPsocket&) = delete;
		TCPsocket& operator=(const TCPsocket&) = delete;

		TCPsocket(const char* targetHost, const char* targetPort){
			addrinfo hints;
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET6;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_socktype = SOCK_STREAM;
			if(getaddrinfo(targetHost, targetPort, &hints, &targetAddr)!=0){
				std::string msg = "\n\ngetaddrinfo failed due to " + (std::string) strerror(errno);
				throw std::runtime_error(msg);
			}
			sockFd = socket(targetAddr->ai_family, targetAddr->ai_socktype, targetAddr->ai_protocol);
			if(connect(sockFd, targetAddr->ai_addr, targetAddr->ai_addrlen)!=0){
				std::string msg = "\n\nconnect failed due to " + (std::string) strerror(errno);
				throw std::runtime_error(msg);
			}
		}

		TCPsocket(TCPsocket&& s){
			targetAddr = s.targetAddr;
			s.targetAddr = nullptr;
			sockFd = s.sockFd;
			s.sockFd = -1;
		}

		TCPsocket& operator=(TCPsocket&& s){
			freeaddrinfo(this->targetAddr);
			targetAddr = s.targetAddr;
			s.targetAddr = nullptr;
			sockFd = s.sockFd;
			s.sockFd = -1;
			return *this;
		}

		~TCPsocket(){
			if(this->sockFd>=0){
				close(this->sockFd);
			}
			if(this->targetAddr!=nullptr){
				freeaddrinfo(this->targetAddr);
			}
		}

};
