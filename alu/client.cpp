#include "header.h"

inline const char *const BoolToString(bool b) return b ? "1" : "0";

void callServer(int serverSocket, bool state){
	request req;
	strncpy(req.type, "SERVER_STATE", 14);
	strncpy(req.msg, BoolToString(state), 2);
	send_request(serverSocket, &req);
}

bool newState(int livingCells, bool state){
	bool newState;
	switch (livingCells){
		case(livingCells > 3):
			newState = false
			break;

		case(livingCells < 2 ):
			newState = false
			break;

		case(livingCells == 3):
			newState = true
			break;
			
		case(livingCells == 2 && state):
			newState = true
			break;
	}
	return newState;
}

int acceptNeighbours(sockaddr_in addr, int s, vector<int> &listenNeighbours){
	int t = sizeof(addr);
	for (;;){
		int socket = accept(s, (struct sockaddr *)&addr, (socklen_t *)&t);
		if (socket == -1){
			perror("validating incoming connection request");
			exit(1);
		}
		listenNeighbours.push_back(socket);	
	}
}

int connectNeighbour(int gate){
	struct sockaddr_in remote;
	int neighboursSocket;
	if ((neighboursSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("creating socket");
		exit(1);
	}
	remote.sin_family = AF_INET;
	remote.sin_port = htons(gate);
	remote.sin_addr.s_addr = INADDR_ANY;
	int x = connect(neighboursSocket, (struct sockaddr *)&remote, sizeof(remote));
	if (x == -1){
		perror("connecting");
		exit(1);
	}

	return neighboursSocket;
}

void connectNeighbours(vector<int> neighbours, vector<int> &neighboursChat){
	for (int i = 0; i < neighbours.size(); ++i)
		neighboursChat.push_back(connectNeighbour(neighbours[i]));
}

void changeNeighbours(string neighbours, vector<int> &oldNeighbours){
	const char limit = ' ';
	stringstream ss(neighbours);
	string x;
	while (std::getline(ss, x, limit)){
		if (x != "") oldNeighbours.push_back(atoi(x.c_str()));
	}
}

void listenNeighbour(vector<int> &neighbourSocket, bool &livingCell, int server){
	int livingCells = 0;
	for (int i = 0; i < neighbourSocket.size(); ++i){
		int neighbour = neighbourSocket[i];
		request req;
		get_request(req, neighbours);
		if (strncmp(req.msg, "1", 2) == 0) livingCells++;
	}

	livingCell = newState(livingCells, livingCell);
	callServer(server, livingCell);
}

void giveInfo(int neighbour, bool state){
	request req;
	strncpy(req.type, "STATE", 7);
	strncpy(req.msg, BoolToString(state), 2);
	send_request(neighbour, &req);
}

void answerNeighbour(vector<int> &neighbourSocket, bool state){
	for (int i = 0; i < neighbourSocket.size(); ++i){
		giveInfo(neighbourSocket[i], state);
	}
}

int main(int argc, char const *argv[]){
	bool livingCell = atoi(argv[1]) % 2 == 0;
	int socket_fd;
	int socketEscucha;
	struct sockaddr_in remote;
	struct sockaddr_in local;
	struct in_addr addr;
	vector<thread> threads;
	vector<int> neighbours;
	vector<int> talkNeighbourSocket;
	vector<int> listenNeighbourSocket;
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("creating socket");
		exit(1);
	}

	/* Establecer la dirección a la cual conectarse. */
	remote.sin_family = AF_INET;
	remote.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr));

	/* crea socket */
	if ((socketEscucha = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	/* configura dirección */
	int port = atoi(argv[1]);
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = INADDR_ANY;

	/* linkea socket con dirección */
	if (bind(socketEscucha, (struct sockaddr *)&local, sizeof(local)) < 0){
		perror("bind");
		exit(1);
	}

	/* setea socket a modo "listen"*/
	if (listen(socketEscucha, 10) == -1){
		perror("listen");
		exit(1);
	}

	/* Conectarse. */
	int s = connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote));
	if (s == -1){
		perror("connecting");
		exit(1);
	}

	request req;
	strncpy(req.type, "GATE", 7);
	strncpy(req.msg, to_string(port).c_str(), sizeof(to_string(port).c_str()));
	send_request(socket_fd, &req);
	request reqEstado;
	strncpy(reqEstado.type, "SERVER_STATE", 14);
	strncpy(reqEstado.msg, BoolToString(livingCell), 2);
	send_request(socket_fd, &reqEstado);
	while (1){
		int socket;
		request req;
		get_request(&req, socket_fd);
		if (strncmp(req.type, "NEIGHBOURS", 8) == 0){
			changeNeighbours(string(req.msg), neighbours);
			threads.push_back(thread(connectNeighbours, neighbours, ref(talkNeighbourSocket)));
			threads.push_back(thread(acceptNeighbours, local, socketEscucha, ref(listenNeighbourSocket)));
		}
		if (strncmp(req.type, "TACK", 5) == 0){
			threads.push_back(thread(answerNeighbour, ref(talkNeighbourSocket), livingCell));
			threads.push_back(thread(listenNeighbour, ref(listenNeighbourSocket), ref(livingCell), socket_fd));
		}
	}

	for (unsigned int i = 0; i < threads.size(); i++)threads[i].join();

	return 0;
}