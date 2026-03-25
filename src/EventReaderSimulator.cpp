#include <cerrno>
#include <cstdio>
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
		printf("\n\nEventReader read: %s", fullEvent.logMessage);
	}
}

int main(int argc, char* argv[]){
	EventReader er("localhost", "8081");
	EventCategory ECS[3] =  {"FTE", "WE", "CBA"};
	er.subscribeToEventCategories(3, ECS);
	playTradingInterval(er, 10);
}
