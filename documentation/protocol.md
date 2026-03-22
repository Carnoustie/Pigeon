This material covers the communication protocol used in the communication between EventWriter -> EventBroker -> EventReader.

The EventBroker imposes the following messaging format for all Events used in Pigeon:

| Field | Type | Description |
|:------|:----:|------------:|
| timeStamp|uint64_t| Unix time of Event in nanoseconds|
| logMessage|char[1024]| Event log message|
| payLoad|uint8_t[MAX]| Event data in raw bytes*|

\* The payload is any arbitrary struct preservable over TCP communication such that the EventWriter can decompose its contents if it knows the struct type. This means that its fields must be primitive types such as char, char[], int, int[], double, double[] etc., and the EventWriter and EventReader must agree upon the format of payload, so that the EventReader can process the payload in a meaningful way once receiving events. The arbitrariness of the payload format allows Pigeon to be used across a variety of applications with various formatting needs, and the maximum payload size MAX is configurable to further adapt Pigeon to latency/memory/bandwidth demands. This means that EventWriters and EventReaders that target/subscribe to the same EventBuffer in EventBroker must be compiled with the same struct definition, since there is no schema negotiation at runtime. If the payload struct changes, both sides must be recompiled and redeployed together.