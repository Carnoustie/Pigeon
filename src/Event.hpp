#include <cstdint>
#define MAX_LOG 4096
#define MAX_PAYLOAD 16384
#define MAX_LEN_EVENT_CATEGORY_NAME 32

//Base event struct
struct Event{
	uint64_t timeStamp;
	char logMessage[MAX_LOG];
	uint8_t payLoad[MAX_PAYLOAD];
};

//Event Category Array
typedef char EventCategory[MAX_LEN_EVENT_CATEGORY_NAME];
