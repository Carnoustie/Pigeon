#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "EventReader.hpp"
#include "FinancialTradingEvent.hpp"


void playTradingInterval(EventReader& er, int intervalInSeconds){
	Event fullEvent;
	FTP tradingEvent;

	while(1){
		er.readEvent();
	}
}

int main(int argc, char* argv[]){
	const char* host = getenv("BROKER_HOST") ? getenv("BROKER_HOST") : "localhost";
	EventReader er(host, "8081");
	EventCategory ECS[3] =  {"FTE", "WE", "CBA"};
	er.subscribeToEventCategories(3, ECS);
	playTradingInterval(er, 10);
}
