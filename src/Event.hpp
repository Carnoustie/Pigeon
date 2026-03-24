#include <cstdint>
#define MAX_LOG 4096
#define MAX_PAYLOAD 16384

//Base event struct
struct Event{
	uint64_t timeStamp;
	char logMessage[MAX_LOG];
	uint8_t payLoad[MAX_PAYLOAD];
};
