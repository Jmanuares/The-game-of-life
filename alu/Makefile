# No anda
.PHONY: all clean

CC = g++

CFLAGS  = -g -Wall

# The build target 
TARGET = server client

OTHER_OBJS = utils.cpp

all: server client

server: server
	$(CC) $(CFLAGS) -o server server.cpp $(OTHER_OBJS)

client: client
	$(CC) $(CFLAGS) -o client client.cpp $(OTHER_OBJS)

clean:
	$(RM) $(TARGET)