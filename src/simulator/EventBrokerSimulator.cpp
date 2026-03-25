#include "EventBroker.hpp"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	EventBroker evtBrkr = EventBroker(1,1);

	evtBrkr.AcceptEventWriters();
	evtBrkr.AcceptEventReaders();
}
