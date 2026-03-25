#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <random>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include "EventWriter.hpp"
#include "FinancialTradingEvent.hpp"

std::mt19937 randomEngine(19);
std::normal_distribution<double> gaussian(0,1);
std::uniform_int_distribution<> uniformSampler(500, 1500);

void playTradingInterval(EventWriter& ew){

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
	Event fullEvent;
	fullEvent.timeStamp = t;
	memcpy(fullEvent.logMessage, &logString, sizeof(logString));
	memcpy(fullEvent.payLoad, &tradingEvent, sizeof(FTP));
	int bytesSent;
	while(1){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		t = time(nullptr);
		tt = (std::time_t) t;
		bytesWritten = strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&tt));
		bytesWritten = sprintf(logString, "\n\nAt %s, %s purchased %d units of %s from %s at final execution price $%f", timeString, tradingEvent.buyer, tradingEvent.numUnits, tradingEvent.ticker, tradingEvent.seller, tradingEvent.price);
		tradingEvent.price = tradingEvent.price + gaussian(randomEngine);
		tradingEvent.numUnits = uniformSampler(randomEngine);
		fullEvent.timeStamp = t;
		memcpy(fullEvent.logMessage, &logString, sizeof(logString));
		memcpy(fullEvent.payLoad, &tradingEvent, sizeof(FTP));
		ew.writeEvent(fullEvent);
	}
}

int main(int argc, char* argv[]){
	EventWriter ew("localhost", "8080");
	EventCategory ECS[3] = {"FTE", "WE", "CBA"};
	ew.announceEventCategories(3, ECS);
	playTradingInterval(ew);
}
