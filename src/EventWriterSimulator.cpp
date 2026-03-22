#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include "Event.hpp"



struct FTP{
	char buyer[256];
	char seller[256];
	char ticker[256];
	int numUnits;
	double price;
};

//Financial Trading Event
struct FTE{
	uint64_t timestamp;
	char logMessage[4096];
	FTP payload;
};

std::mt19937 randomEngine(19);
std::normal_distribution<double> gaussian(0,1);
std::uniform_int_distribution<> uniformSampler(500, 1500);

void playTradingInterval(int recvSockFD, addrinfo recv, int durationInSeconds){

	int duration = durationInSeconds*1000000;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> current = start;

	FTP tradingEvent = {
		"Citadel Securities",
		"Morgan Stanley",
		"NVIDIA",
		999,
		197.92
	}

	char logString[4096];
	int bytesWritten = fprintf(logString, "\n\nAt %s, %s purchased %d units of %s from %s at price %f", current, tradingEvent.buyer, tradingEvent.numUnits, tradingEvent.ticker, tradingEvent.seller, tradingEvent.price)

	FTE fullEvent = {
		start,
		logString,
		FTP
	};

	int bytesSent;

	FTE eventBuffer[1];
	eventBuffer[0] = FTE;

	while(std::chrono::duration_cast<std::chrono::milliseconds>(current-start).now()<duration){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		bytesWritten = fprintf(logString, "\n\nAt %s, %s purchased %d units of %s from %s at price %f", current, tradingEvent.buyer, tradingEvent.numUnits, tradingEvent.ticker, tradingEvent.seller, tradingEvent.price);
		tradingEvent.price = tradingEvent.price + gaussian(randomEngine);
		tradingEvent.numUnits = uniformSampler(randomEngine);
		fullEvent.payload = FTP;
		eventBuffer[0] = FTE;
		bytesSent = recv(recvSockFD, eventBuffer, sizeof(eventBuffer), 0);
	}
}

int main(int argc, char* argv[]){

	struct addrinfo hints;
	struct addrinfo result;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET6;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo("localhost", "8080", &hints, &result)!=0){
		printf("Failed getting adress due to %s", strerror(errno));
	}

	int sockFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if(connect(sockFD, result->ai_addr, result->ai_addrlen)!=0){
		printf("Failed connecting due to %s", strerror(errno));
	}
}
