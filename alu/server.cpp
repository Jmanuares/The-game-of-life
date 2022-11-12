#include "header.h"
#include <thread>
using namespace std;

vector<vector<int>> neighbours(int x, int y, vector<vector<int>> &availableNeighbours, int boardCells){
	vector<vector<int>> neighbours;
	for (size_t i = 0; i < 8; i++){
		int posX = x + availableNeighbours[i][0];
		int posY = y + availableNeighbours[i][1];
		if (posX > -1 && posY > -1 && posX < boardCells && posY < boardCells)
			neighbours.push_back(vector<int>{posX, posY});
	}
	return neighbours;
}

string gatherNeighbours(vector<vector<int>> &playerPorts, vector<vector<int>> &posNeighbours){
	string neighbours;

	for (int i = 0; i < posNeighbours.size(); ++i){
		neighbours += " ";
		neighbours += to_string(playerPorts[posNeighbours[i][0]][posNeighbours[i][1]]);
	}
	return neighbours;
}

void createGroups(vector<vector<int>> &playerSockets, vector<vector<int>> &ports){
	vector<vector<int>> availableNeighbours;
	availableNeighbours.push_back(vector<int>{1, 1});
	availableNeighbours.push_back(vector<int>{0, 1});
	availableNeighbours.push_back(vector<int>{-1, 1});
	availableNeighbours.push_back(vector<int>{1, 0});
	availableNeighbours.push_back(vector<int>{-1, 0});
	availableNeighbours.push_back(vector<int>{1, -1});
	availableNeighbours.push_back(vector<int>{0, -1});
	availableNeighbours.push_back(vector<int>{-1, -1});

	for (size_t i = 0; i < playerSockets.size(); i++){
		for (size_t j = 0; j < playerSockets.size(); j++){
			vector<vector<int>> posNeighbours = neighbours(i, j, availableNeighbours, playerSockets.size());
			string neighbours = gatherNeighbours(ports, posNeighbours);
			request req;
			strncpy(req.type, "CELLS", 6);
			strncpy(req.msg, neighbours.c_str(), 256);
			send_request(playerSockets[i][j], &req);
		}
	}
}

bool drawBoard(vector<vector<int>> &playerSockets){
	string board = "";
	int deads = 0;
	for (size_t i = 0; i < playerSockets.size(); i++){
		board+= "\n";
        for (size_t j = 0; j < playerSockets.size(); j++){
			request req;
            get_request(&req,playerSockets[i][j]);
			if(strncmp(req.msg, "0", 2) == 0) deads++;
			board+= " ";
			board+= req.msg;
			board+= " ";
        }   
    }
	cout << board << endl;

	if(deads == pow(playerSockets.size(), 2))
		return false;
	
	return true;
}

void ticks(vector<vector<int>> &playerSockets, sem_t &endgame, vector<thread>& threads)
{
	int aux = 0;

	while (1)
	{
		sleep(4);
		if (drawBoard(playerSockets))
		{
			string tick = "T" + to_string(aux);
			request req;
			strncpy(req.msg, tick.c_str(), sizeof(tick.c_str()));
			strncpy(req.type, "tick", 5);
			broadcast(playerSockets, &req);
			aux++;
		}
		else
		{
			cout << "Todas las celulas murieron :( " << endl;
			request req;
			strncpy(req.msg, "El juego termino", 17);
			strncpy(req.type, "END", 4);
			broadcast(playerSockets, &req);
			sem_post(&endgame);
			exit(0);
		}
		
	}
}

bool playerCount(vector<vector<int>> &playerSockets, vector<int> &socketsOn){
	if (socketsOn.size() == 9){
		cout << "El juego esta listo" << endl;
		sleep(3);
		int aux = 0;
		for (size_t i = 0; i < playerSockets.size(); i++){
			for (size_t j = 0; j < playerSockets.size(); j++){
				playerSockets[i][j] = socketsOn[aux];
				aux++;
			}
		}
		return true;
	}
	return false;
}

int main(int argc, char const *argv[]){
	int s;
	struct sockaddr_in local;
	struct sockaddr_in remote;
	vector<thread> threads;
	vector<int> sockets;
	vector<vector<int>> playerSockets(3, vector<int>(3));
	vector<vector<int>> ports(3, vector<int>(3));

	sem_t clientSem;
	sem_init(&clientSem, 0, 0);
	sem_t gameReady;
	sem_init(&gameReady, 0, 0);
	sem_t endgame;
	sem_init(&endgame, 0, 0);
	/* crea socket */
	s = set_acc_socket(atoi(argv[1]));
	int t = sizeof(remote);
	
	threads.push_back(thread(accept_conns, s, ref(sockets), ref(clientSem)));
	
	for (size_t i = 0;  i < 9; i++){
		sem_wait(&clientSem);
	};
	
	if(playerCount(playerSockets, sockets)){
		for (size_t i = 0; i < playerSockets.size(); i++){
			for (size_t j = 0; j < playerSockets.size(); j++){
				request req;
				get_request(&req, playerSockets[i][j]);
				char gates[sizeof(req.msg)];
				strncpy(gates, req.msg, sizeof(req.msg));
				ports[i][j] = atoi(gates);
			}
		}
		createGroups(playerSockets, ports);
		for (size_t i = 0; i < playerSockets.size(); i++){
			for (size_t j = 0; j < playerSockets.size(); j++){
				request req;
				get_request(&req,playerSockets[i][j]);
				if(strncmp(req.type, "READY", 6) == 0) sem_post(&gameReady);
			}
			
		}
		for (size_t i = 0;  i < 9; i++){
			sem_wait(&gameReady);
		}
		threads.push_back(thread(ticks, ref(playerSockets), ref(endgame),ref(threads)));
	}
		
	sem_wait(&endgame);
	for (unsigned int i = 0; i < threads.size(); i++)
		threads[0].join();
	
	return 0;
}