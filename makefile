# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g

# Source files (one for each executable)
SRCS = Kosaraju.cpp Kosaraju-List.cpp Kosaraju-Deque.cpp Kosaraju-3.cpp Kosaraju-4.cpp Kosaraju-7-Threads.cpp 

# Object files (one for each source file)
OBJS = $(SRCS:.cpp=.o)

# Executables (one for each source file)
EXES = $(SRCS:.cpp=.exe)

# Default target: build all executables
all: $(EXES) reactor proactor kosaraju10

reactor: reactor.cpp Kosaraju-5.cpp
	$(CC) $(CFLAGS) Kosaraju-5.cpp reactor.cpp -o Kosaraju-Reactor-5

proactor: proactor.cpp Kosaraju-Proactor.cpp
	$(CC) $(CFLAGS) Kosaraju-Proactor.cpp proactor.cpp -o Kosaraju-Proactor

kosaraju10: Kosaraju-10.cpp proactor.cpp
	$(CC) $(CFLAGS) Kosaraju-10.cpp proactor.cpp -o Kosaraju-10

# Compile each source file into an object file
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link each object file into an executable
%.exe: %.o
	$(CC) $(CFLAGS) $< -o $@

# Clean up object files and executables
clean:
	rm -f $(OBJS) $(EXES) Kosaraju-10 Kosaraju-Reactor-5 Kosaraju-Proactor
