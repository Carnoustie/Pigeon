#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class ClientTCPsocket{

	private:
		addrinfo* targetAddr = nullptr;
		int sockFd = -1;
	public:
		ClientTCPsocket(const ClientTCPsocket&) = delete;
		ClientTCPsocket& operator=(const ClientTCPsocket&) = delete;

		ClientTCPsocket(): targetAddr{nullptr}, sockFd{-1} {}
		ClientTCPsocket(const char* targetHost, const char* targetPort){
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

		ClientTCPsocket(ClientTCPsocket&& s){
			targetAddr = s.targetAddr;
			s.targetAddr = nullptr;
			sockFd = s.sockFd;
			s.sockFd = -1;
		}

		ClientTCPsocket& operator=(ClientTCPsocket&& s){
			freeaddrinfo(this->targetAddr);
			targetAddr = s.targetAddr;
			s.targetAddr = nullptr;
			sockFd = s.sockFd;
			s.sockFd = -1;
			return *this;
		}

		~ClientTCPsocket(){
			if(this->sockFd>=0){
				close(this->sockFd);
			}
			if(this->targetAddr!=nullptr){
				freeaddrinfo(this->targetAddr);
			}
		}

};


class ServerTCPsocket{

	private:
		addrinfo* serverAddr = nullptr;
		int sockFd = -1;
	public:
		ServerTCPsocket(const ServerTCPsocket&) = delete;
		ServerTCPsocket& operator=(const ServerTCPsocket&) = delete;

		ServerTCPsocket(): serverAddr{nullptr}, sockFd{-1} {}
		ServerTCPsocket(const char* serverPort, int backlog){
			addrinfo hints;
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET6;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_socktype = SOCK_STREAM;
			if(getaddrinfo(NULL, serverPort, &hints, &serverAddr)!=0){
				std::string msg = "\n\ngetaddrinfo failed due to " + (std::string) strerror(errno);
				throw std::runtime_error(msg);
			}
			sockFd = socket(serverAddr->ai_family, serverAddr->ai_socktype, serverAddr->ai_protocol);

			if(bind(sockFd, serverAddr->ai_addr, serverAddr->ai_addrlen)!=0){
				std::string msg = "\n\nbind failed due to " + (std::string) strerror(errno);
				throw std::runtime_error(msg);
			}

			if(listen(sockFd, backlog)!=0){
				std::string msg = "\n\nlisten failed due to" + (std::string) strerror(errno);
				throw std::runtime_error(msg);
			}
		}


		ServerTCPsocket(ServerTCPsocket&& s){
			serverAddr = s.serverAddr;
			s.serverAddr = nullptr;
			sockFd = s.sockFd;
			s.sockFd = -1;
		}

		ServerTCPsocket& operator=(ServerTCPsocket&& s){
			freeaddrinfo(this->serverAddr);
			serverAddr = s.serverAddr;
			s.serverAddr = nullptr;
			sockFd = s.sockFd;
			s.sockFd = -1;
			return *this;
		}

		~ServerTCPsocket(){
			if(this->sockFd>=0){
				close(this->sockFd);
			}
			if(this->serverAddr!=nullptr){
				freeaddrinfo(this->serverAddr);
			}
		}

};
