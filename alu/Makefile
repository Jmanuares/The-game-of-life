.PHONY: all clean

CC = g++

CFLAGS  = -g -Wall

# The build target 
TARGET = server client

OTHER_OBJS = utils.cpp

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp $(OTHER_OBJS)

client: client.cpp
	$(CC) $(CFLAGS) -o client client.cpp $(OTHER_OBJS)

clean:
	$(RM) $(TARGET)