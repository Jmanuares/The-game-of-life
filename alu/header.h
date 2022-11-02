#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <iostream>
#include <sys/sendfile.h>
#include <vector>
#include <semaphore.h>
#include <mutex>
#include <sys/wait.h>
#include <cassert>
#include <time.h>    
#include <sstream>
#include <math.h>
#define PORT 5031

using namespace std;

struct request{
	char type[10];
	char msg[256];
};


// Funciones utiles sugeridas
int read_sock(char str[], int s);
void broadcast(vector<vector<int>> &sockets, struct request *req);
void get_request(struct request* req, int s);
void send_request(int s, struct request *req);
void accept_conns(int s, vector<int>& v, sem_t& clientSem);
int set_acc_socket(int lsn_port);
int read_socket(int s);
void connection_handler(int dSocket);


