# Architectural Diary

Collection of design problems encountered while building Pigeon

## 04 April 2026 - Contention between polling and Fanout

While reading about cutting-edge message-brokers and listening to experts discussing them on podcast, they appear to support fanout from topics to consumers on the one hand, while consumer reads are pull-based on the other hand. As I was building Pigeon with this in mind, these features seemingly stand in contention to one another. Fanout of messages would clearly be easier if the EventBroker initiates propagation of events to EventReaders, i.e it follows a push-based design. Then, the broker could simply walk the list of subscribers for a given EventQueue and send a given message to all subscribers over the network. Meanwhile, such a push-based abandons one of the key characteristics of EventBrokers - its role as a buffer between EventWriters and EventReaders. Its clear that the optionality for EventReaders to read Events at a time of their discretion is fundamental to any useful message broker - its perhaps the main existential purpose of message brokers in the first place. We thus face a competition between two design elements:

### Tradeoff TO_1: _*A pull-based design from EventReaders to EventQueues makes Event fanout inconvenient*_

### Pigeon Solution

A possible solution to this problem is to accept the tradeoff at face value, and add complexity and possible latency to support both competing interest via expanding the communication protocol. This could for example mean that track is kept of when all subscribers have read a given message, after which it is removed from the message queue but remains on persistent storage for backup purposes.

### Kafka Solution

Kafka adresses this tradeoff by .....