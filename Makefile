CC=g++-6
CFLAGS=-Wall -Wextra -std=c++14
CFLAGS_DEBUG=-g
LINK_FLAGS=-lpthread

PORT=12345
SERVER_IP=127.0.01

all: client server

client: src/client.cpp src/util.cpp src/util.h
	$(CC) -o bin/client src/client.cpp src/util.cpp $(LINK_FLAGS) $(CFLAGS_DEBUG) $(CFLAGS)

server: src/server.cpp src/util.cpp src/util.h
	$(CC) -o bin/server src/server.cpp src/util.cpp $(LINK_FLAGS) $(CFLAGS_DEBUG) $(CFLAGS)

run_server:
	bin/server $(PORT)

run_client:
	bin/client $(SERVER_IP) $(PORT)

spawn_clients:
	scripts/spawn_clients.sh

clean:
	rm -rf bin/server bin/client
