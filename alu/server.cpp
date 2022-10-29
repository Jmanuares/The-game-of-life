#include "header.h"
#include <thread>
using namespace std;

vector<vector<int>> neighbours(int x, int y, vector<vector<int>> &availableNeighbours, int boardCells)
{
	vector<vector<int>> neighbours;
	for (size_t i = 0; i < 8; i++)
	{
		int posX = x + availableNeighbours[i][0];
		int posY = y + availableNeighbours[i][1];
		if (posX > -1 && posY > -1 && posX < boardCells && posY < boardCells)
		{
			neighbours.push_back(vector<int>{posX, posY});
		}
	}
	return neighbours;
}

string gatherNeighbours(vector<vector<int>> &playerPorts, vector<vector<int>> &posNeighbours)
{
	string neighbours;

	for (int i = 0; i < posNeighbours.size(); ++i)
	{
		neighbours += " ";
		neighbours += to_string(playerPorts[posNeighbours[i][0]][posNeighbours[i][1]]);
	}
	return neighbours;
}

void createGroups(vector<vector<int>> &playerSockets, vector<vector<int>> &ports)
{
	vector<vector<int>> availableNeighbours;
	availableNeighbours.push_back(vector<int>{1, 1});
	availableNeighbours.push_back(vector<int>{0, 1});
	availableNeighbours.push_back(vector<int>{-1, 1});
	availableNeighbours.push_back(vector<int>{1, 0});
	availableNeighbours.push_back(vector<int>{-1, 0});
	availableNeighbours.push_back(vector<int>{1, -1});
	availableNeighbours.push_back(vector<int>{0, -1});
	availableNeighbours.push_back(vector<int>{-1, -1});

	for (size_t i = 0; i < playerSockets.size(); i++)
	{
		for (size_t j = 0; j < playerSockets.size(); j++)
		{
			vector<vector<int>> posNeighbours = neighbours(i, j, availableNeighbours, playerSockets.size());
			string neighbours = gatherNeighbours(ports, posNeighbours);
			request req;
			strncpy(req.type, "NEIGHBOURS", 8);
			strncpy(req.msg, neighbours.c_str(), 256);
			send_request(playerSockets[i][j], &req);
		}
	}
}

void drawBoard(vector<vector<int>> &playerSockets)
{
	string board = "";
	for (size_t i = 0; i < playerSockets.size(); i++)
    {
		board+= "\n";
        for (size_t j = 0; j < playerSockets.size(); j++)
        {
			request req;
            get_request(&req,playerSockets[i][j]);
			board+= " ";
			board+= req.msg;
			board+= " ";
        }   
    }
	cout << board << endl;
}

void ticks(vector<vector<int>> &playerSockets)
{
	int aux = 0;
	while (1)
	{
		drawBoard(playerSockets);
		string tack = "T" + to_string(aux);
		int n = tack.length();
		char time[n + 1];
		strcpy(time, tack.c_str());
		request req;
		strncpy(req.msg, time, sizeof(time));
		strncpy(req.type, "TACK", 10);
		broadcast(playerSockets, &req);
		aux++;
		sleep(4);
	}
}

bool playerCount(vector<vector<int>> &playerSockets, vector<int> &socketsOn, int newSockets)
{
	socketsOn.push_back(newSockets);
	if (socketsOn.size() == 9)
	{
		int aux = 0;
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				playerSockets[i][j] = socketsOn[aux];
				aux++;
			}
		}
		return true;
	}
	return false;
}

int main(void)
{
	int s;
	struct sockaddr_in local;
	struct sockaddr_in remote;
	vector<thread> threads;
	vector<int> sockets;
	vector<vector<int>> playerSockets(3, vector<int>(3));
	vector<vector<int>> ports(3, vector<int>(3));

	/* crea socket */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	/* configura dirección */
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr.s_addr = INADDR_ANY;

	/* linkea socket con dirección */
	if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(1);
	}

	/* setea socket a modo "listen"*/
	if (listen(s, 10) == -1)
	{
		perror("listen");
		exit(1);
	}

	int t = sizeof(remote);
	int socket;
	for (;;)
	{
		if ((socket = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1)
		{
			perror("validating incoming connection request");
			exit(1);
		}

		if (playerCount(playerSockets, sockets, socket))
		{
			for (size_t i = 0; i < playerSockets.size(); i++)
			{
				for (size_t j = 0; j < playerSockets.size(); j++)
				{
					request neighboursRequest;
					get_request(&neighboursRequest, playerSockets[i][j]);
					char gates[sizeof(neighboursRequest.msg)];
					strncpy(gates, neighboursRequest.msg, sizeof(neighboursRequest.msg));
					ports[i][j] = atoi(gates);
				}
			}
			createGroups(playerSockets, ports);
			threads.push_back(thread(ticks, ref(playerSockets)));
		}
	}

	for (unsigned int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	return 0;
}
