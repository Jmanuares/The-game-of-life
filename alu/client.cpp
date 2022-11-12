#include "header.h"

inline const char *const BoolToString(bool b){
	return b ? "1" : "0";
}
	

void callServer(int serverSocket, bool state){
	request req;
	strncpy(req.type, "STATE", 6);
	strncpy(req.msg, BoolToString(state), 2);
	send_request(serverSocket, &req);
}

bool newState(int livingCells, bool state){
	bool newState;
	if(livingCells > 3){
		newState = false;
	}
	if(livingCells < 2 ){
		newState = false;
	}
	if (livingCells == 3){
		newState = true;
	}
	if(livingCells == 2 && state){
		newState = true;
	}
	
	
	return newState;
}

void acceptNeighbours(sockaddr_in addr, int s, vector<int> &listenNeighbours, int n, sem_t &sockContinue){
	int t = sizeof(addr);
	for (size_t i = 0; i < n; i++){
		int socket = accept(s, (struct sockaddr *)&addr, (socklen_t *)&t);
		if (socket == -1){
			perror("validating incoming connection request");
			exit(1);
		}
		listenNeighbours.push_back(socket);	
		sem_post(&sockContinue);
	}
}

void changeNeighbours(string neighbours, vector<int> &oldNeighbours){
	const char limit = ' ';
	string x;
	stringstream ss(neighbours);
	while (std::getline(ss, x, limit)){
		if (x != "") oldNeighbours.push_back(atoi(x.c_str()));
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

void connectNeighbours(vector<int> neighbours, vector<int> &neighboursChat, sem_t &sockContinue){
	for (int i = 0; i < neighbours.size(); ++i)
		neighboursChat.push_back(connectNeighbour(neighbours[i]));

	sem_post(&sockContinue);
}

void listenNeighbour(vector<int> &neighbourSocket, bool &livingCell, int server){
	int livingCells = 0;
	for (int i = 0; i < neighbourSocket.size(); ++i){
		int neighbour = neighbourSocket[i];
		request req;
		get_request(&req, neighbour);
		if (strncmp(req.msg, "1", 2) == 0) livingCells++;
	}

	livingCell = newState(livingCells, livingCell);
	callServer(server, livingCell);
}

void giveInfo(int neighbour, bool state){
	request req;
	strncpy(req.type, "STATE", 6);
	strncpy(req.msg, BoolToString(state), 2);
	send_request(neighbour, &req);
}

void answerNeighbour(vector<int> &neighbourSocket, bool state){
	for (int i = 0; i < neighbourSocket.size(); ++i){
		giveInfo(neighbourSocket[i], state);
	}
}

int main(int argc, char const *argv[]){
	int mainSocket;
	int listenSocket;
	bool livingCell = atoi(argv[1]) % 2 == 0;
	struct sockaddr_in remote;
	struct sockaddr_in local;
	vector<thread> threads;
	vector<int> neighbours;
	vector<int> socketNeighboursComm;
	vector<int> ListenNeighboursSocket;
	sem_t sockContinue;
	sem_init(&sockContinue, 0, 0);

	/*Crea socket que conectara al servidor*/
	mainSocket = connectNeighbour(atoi(argv[2]));

	/* Crea socket para escuchar vecinos */
	int port = atoi(argv[1]);
	listenSocket = set_acc_socket(port);

	/*Manda informacion necesaria para comenzar el juego.*/
	request req;
	strncpy(req.type, "GATE", 5);
	strncpy(req.msg, to_string(port).c_str(), sizeof(to_string(port).c_str()));
	send_request(mainSocket, &req);
	request reqEstado;

	/*Recibo request de server hasta que termine el juego.*/
	while (1){
		int socket;
		request req;
		get_request(&req, mainSocket);
		if (strncmp(req.type, "END", 4) == 0){
			for (int i = 0 ; i < ListenNeighboursSocket.size(); i++)
				close(ListenNeighboursSocket[i]);

			for (unsigned int i = 0; i < threads.size(); i++)
				threads[0].join();

			exit(1);
		}
		if (strncmp(req.type, "CELLS", 6) == 0){
			changeNeighbours(string(req.msg), neighbours);

			threads.push_back(thread(connectNeighbours, neighbours, ref(socketNeighboursComm), ref(sockContinue)));
			threads.push_back(thread(acceptNeighbours, local, listenSocket, ref(ListenNeighboursSocket), neighbours.size(), ref(sockContinue)));
			for (size_t i = 0; i < 2; i++){
				sem_wait(&sockContinue);
			}
			request req;
			strncpy(req.type, "READY", 6);
			strncpy(req.msg, "ok", 3);
			send_request(mainSocket, &req);
			request reqEstado;
			strncpy(reqEstado.type, "STATE", 6);
			strncpy(reqEstado.msg, BoolToString(livingCell), 2);
			send_request(mainSocket, &reqEstado);
		}
		if (strncmp(req.type, "tick", 5) == 0){
			threads.push_back(thread(answerNeighbour, ref(socketNeighboursComm), livingCell));
			threads.push_back(thread(listenNeighbour, ref(ListenNeighboursSocket), ref(livingCell), mainSocket));
		}

		if (strncmp(req.type, "END", 4) == 0){
			break;
		}
	}

	for (unsigned int i = 0; i < threads.size(); i++)
		threads[i].join();

	return 0;
}