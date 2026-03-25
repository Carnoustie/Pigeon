#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <random>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include "Event.hpp"
#include "TCPsocket.hpp"

//Financial Trading Event Payload
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

void playTradingInterval(int recvSockFD, addrinfo* recv){

	FTP tradingEvent = {
		"Citadel Securities",
		"Morgan Stanley",
		"NVIDIA",
		999,
		197.92
	};

	char logString[4096];
	char timeString[64];
	uint64_t t = time(nullptr);
	std::time_t tt = (std::time_t) t;
	int bytesWritten = strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&tt));
	bytesWritten = sprintf(logString, "\n\nAt %s, %s purchased %d units of %s from %s at final execution price $%f", timeString, tradingEvent.buyer, tradingEvent.numUnits, tradingEvent.ticker, tradingEvent.seller, tradingEvent.price);
	FTE fullEvent;
	fullEvent.timestamp = t;
	memcpy(fullEvent.logMessage, logString, sizeof(logString));
	fullEvent.payload = tradingEvent;
	int bytesSent;
	while(1){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		t = time(nullptr);
		tt = (std::time_t) t;
		bytesWritten = strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&tt));
		bytesWritten = sprintf(logString, "\n\nAt %s, %s purchased %d units of %s from %s at final execution price $%f", timeString, tradingEvent.buyer, tradingEvent.numUnits, tradingEvent.ticker, tradingEvent.seller, tradingEvent.price);
		tradingEvent.price = tradingEvent.price + gaussian(randomEngine);
		tradingEvent.numUnits = uniformSampler(randomEngine);
		fullEvent.timestamp = t;
		memcpy(fullEvent.logMessage, logString, sizeof(logString));
		fullEvent.payload = tradingEvent;
		bytesSent = sendto(recvSockFD, &fullEvent, sizeof(fullEvent), 0, recv->ai_addr, recv->ai_addrlen);
	}
}

int main(int argc, char* argv[]){
	ClientTCPsocket socket("localhost", "8080");
	playTradingInterval(socket.sockFd, socket.targetAddr);
}
