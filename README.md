# Operation System Course: Kosaraju Assignment

This repository contains the solution for the Kosaraju Assignment in the Operating System Course.

## Description

In our Operating Systems assignment, we explored the implementation of multi-client communication using diverse paradigms such as Reactor, Proactor, Select, Poll, and referenced Beej's Chat for practical illustration. This exercise focused on understanding and implementing efficient socket programming techniques to manage simultaneous client connections. By comparing and contrasting these paradigms, we aimed to enhance our grasp of asynchronous I/O and event-driven architectures, essential for developing scalable and robust network applications in real-world scenarios.

### Steps and Descriptions:

- **Step 1:**
  - Kosaraju main program:
  - `Kosaraju.cpp`
  - Implementation of the main Kosaraju algorithm for finding strongly connected components in a directed graph.

- **Step 2:**
  - Graph represented in different data structures to differentiate profiling results.
  - `Kosaraju-List.cpp`
  - `Kosaraju-Deque.cpp`
  - Experimenting with different data structures (lists, dequeues) to analyze their impact on algorithm performance.

- **Step 3:**
  - StdIn interaction:
  - `Kosaraju-3.cpp`
  - Implementation of interactive command-line interface for user input and graph manipulation.

- **Step 4:**
  - Beej Chat:
  - `Kosaraju-4.cpp`
  - Integration of concepts from Beej's Guide to Network Programming for practical application in socket communication.

- **Step 5 & 6:**
  - Reactor:
  - `Kosaraju-5.cpp`
  - `reactor.cpp`
  - Implementation of the Reactor pattern for handling multiple concurrent clients using event-driven architecture.

- **Step 7:**
  - Multi-threading:
  - `Kosaraju-7-Threads.cpp`
  - Utilization of multi-threading techniques to handle parallel execution of tasks, improving performance and responsiveness.

- **Step 8 & 9:**
  - Proactor:
  - `Kosaraju-Proactor.cpp`
  - `proactor.cpp`
  - Implementation of the Proactor pattern for asynchronous I/O operations, enhancing scalability and resource utilization.

- **Step 10:**
  - Producer Consumer (POSIX Condition):
  - `Kosaraju-10.cpp`
  - `proactor.cpp`
  - Integration of POSIX condition variables for synchronization between producer and consumer threads in a concurrent environment.

---

