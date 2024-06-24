# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g

# Source files (one for each executable)
SRCS = Kosaraju.cpp Kosaraju-List.cpp Kosaraju-Deque.cpp Kosaraju-3.cpp Kosaraju-4.cpp

# Object files (one for each source file)
OBJS = $(SRCS:.cpp=.o)

# Executables (one for each source file)
EXES = $(SRCS:.cpp=.exe)

# Default target: build all executables
all: $(EXES)

# Compile each source file into an object file
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link each object file into an executable
%.exe: %.o
	$(CC) $(CFLAGS) $< -o $@

# Clean up object files and executables
clean:
	rm -f $(OBJS) $(EXES)
